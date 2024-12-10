#include "ptty.h"

#include <QtGlobal> // qDebug
#include <QtDebug> // qDebug overload

extern char** environ; 

Ptty::Ptty(QObject* parent)
    : QObject(parent)
    , m_stop(false)
    , m_readThread(nullptr)
{
    // constructor
    qDebug() << "Constructing Ptty...";
    setupPty();
}

Ptty::~Ptty(){
    // destructor
    stop();
    ::close(m_masterFd);
    ::close(m_slaveFd);
}

bool Ptty::pairMasterSlaveFd(){
    // opening master side of pty
    m_masterFd = ::posix_openpt(O_NOCTTY | O_RDWR);
    if (m_masterFd < 0) {
        perror("posix_openpt");
        return false;
    }

    if (::grantpt(m_masterFd) < 0) {
        perror("grantpt");
        ::close(m_masterFd);
        return false;
    }
    if (::unlockpt(m_masterFd) < 0) {
        perror("unlockpt");
        ::close(m_masterFd);
        return false;
    }

    // opening slave side of pty
    char* slaveFileName = ::ptsname(m_masterFd);
    if (slaveFileName == NULL) {
        perror("ptsname");
        ::close(m_masterFd);
        return false;
    }

    m_slaveFd = ::open(slaveFileName, O_NOCTTY | O_RDWR);
    if (m_slaveFd < 0){
        perror("open(slaveFileName)");
        ::close(m_masterFd);
        return false;
    }

    return true;
}

bool Ptty::setupPty(){
    qDebug() << "Pairing Master-Slave Pty...";
    if (pairMasterSlaveFd() < 0){
        perror("pairMasterSlaveFd");
        return false;
    }
    qDebug() << "Spawning shell session...";
    if (spawnChildProcess() < 0){
        perror("spawnChildProcess");
        return false;
    }

    return true;
}

void Ptty::executeCommand(QString command){
    if (m_masterFd != -1){
        std::lock_guard<std::mutex> lock(m_writeMutex); // protect this thread
        tcflush(m_masterFd, TCIOFLUSH);
        QByteArray cmd = command.toUtf8() + "\n";
        ssize_t written = ::write(m_masterFd, cmd.data(), cmd.size());

        if (written < 0) {
            perror("write(masterFd)");
        } else if (written < cmd.size()) {
            qDebug("Warning: Not all bytes were written to the terminal.");
        }
    }
    // mutex lock auto release when out of scope (behavior of std::lock_guard)
}

// ----- old: this is a busy-wait loop to read input -----
// ----- can eliminate busy-wait with a chrono::sleep ----
// ----- but this messes up the output of real-time ------
// ----- programs like "pstree" or "journalctl" ----------
void Ptty::readLoop(){
    while(!m_stop){
        ssize_t bytesRead = ::read(m_masterFd, resultBuffer, OUTPUT_BUFFER_SIZE-1);
        if (bytesRead > 0) {
            resultBuffer[bytesRead] = '\0';
            emit resultReceivedFromShell(resultBuffer);
        }
        else if (bytesRead < 0) {
            break;
        }
        else{
            // bytesRead == 0 means child process has closed PTY for some reasons
            perror("PTY closed by child process");
            break;
        }
    }
    emit resultReceivedFromShell("Process has exited\n");
}
// -------------------------------------------------------

// ----- new: this is an event-based read loop -----------
// void Ptty::readLoop() {
//     while (!m_stop) {
//         fd_set readFds;
//         FD_ZERO(&readFds);
//         FD_SET(m_masterFd, &readFds);

//         // Use select to wait for data on m_masterFd
//         struct timeval timeout;
//         timeout.tv_sec = 0;         // Seconds
//         timeout.tv_usec = 10000;    // Microseconds (10ms)

//         int rc = select(m_masterFd + 1, &readFds, nullptr, nullptr, &timeout);

//         if (rc > 0 && FD_ISSET(m_masterFd, &readFds)) {
//             std::lock_guard<std::mutex> lock(m_writeMutex);
//             ssize_t bytesRead = read(m_masterFd, resultBuffer, OUTPUT_BUFFER_SIZE - 1);

//             if (bytesRead > 0) {
//                 resultBuffer[bytesRead] = '\0';
//                 emit resultReceivedFromShell(QString::fromStdString(resultBuffer));
                
//             } else if (bytesRead == -1 && errno != EAGAIN) {
//                 perror("read");
//             }
//         } else if (rc == -1) {
//             perror("select");
//         }
//         // If no data is available (rc == 0), loop continues without busy-waiting
//     }
// }


void Ptty::start(){
    m_stop = false;
    m_readThread = new std::thread(&Ptty::readLoop, this);
    m_readThread->detach();
}

void Ptty::stop(){
    m_stop = true;
    if (m_readThread && m_readThread->joinable()){
        m_readThread->join();
        delete m_readThread;
        m_readThread = nullptr;
    }
}

bool Ptty::spawnChildProcess(){
    m_pid = fork();
    if (m_pid < 0) {
        perror("fork");
        return false;
    }
    else if (m_pid > 0){
        // parent process, only need masterFd
        ::close(m_slaveFd);
        return true;
    }

    // setting child process as session leader
    ::setsid();

    // child process, only need slaveFd
    ::close(m_masterFd);

    // set slave side of pty as stdin, stdout, stderr
    ::dup2(m_slaveFd, STDIN_FILENO);
    ::dup2(m_slaveFd, STDOUT_FILENO);
    ::dup2(m_slaveFd, STDERR_FILENO);

    // setting child as the leader of its own group
    if (ioctl(m_slaveFd, TIOCSCTTY, 0) == -1) {
        perror("ioctl(TIOSCTTY)");
        ::_exit(EXIT_FAILURE);
        return false;
    }
    ::setpgid(0, 0);

    const char* nutshellPath = "/home/lanphgphm/Projects/terminix/cpp/appnutshell"; 
    char* const argv[] = {(char*)"appnutshell", nullptr}; 
    ::execve(nutshellPath, argv, environ);

    // if got to here --> fail to exec shell
    perror("execve");
    ::_exit(EXIT_FAILURE);
    return false;
}

void Ptty::sendSignal(int signal){
    if (m_masterFd <= 0){
        perror("Invalid master file descriptor, cannot send signal");
        return;
    }

    pid_t fgPid = tcgetpgrp(m_masterFd);
    if (fgPid == -1){
        perror("tcgetpgrp");
        return;
    }

    int sendSignalResult = killpg(fgPid, signal);

    if (sendSignalResult == -1){
        perror("killpg");
    }
    else {
        qDebug("Signal sent successfully.");
    }
}

