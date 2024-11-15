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
    if (pairMasterSlaveFd() < 0){
        perror("pairMasterSlaveFd");
        return false;
    }
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
            // CONCERN 1.0
            // in case the entire command cant be written at once
            qDebug("Warning: Not all bytes were written to the terminal.");
        }
    }
    // mutex lock auto release when out of scope (behavior of std::lock_guard)
}

void Ptty::readLoop(){
    while(!m_stop){
        ssize_t count = ::read(m_masterFd, resultBuffer, BUFFER_SIZE-1);
        if (count > 0) {
            // ---TODO 1.6: buffer the readBuffer to capture full output----
            // but the output does not have a endOfResult mark character?
            // -------------------------------------------------------------
            resultBuffer[count] = '\0';
            emit resultReceivedFromBash(resultBuffer);
        }
        else if (count < 0) {
            // go inform ScreenController::resultReceivedFromPty()
            // to close session and hide terminal window
            break;
        }
        else{
            // CONCERN 1.1
            // count == 0 means child process has closed PTY for some reasons
            perror("PTY closed by child process");
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME));
    }
    emit resultReceivedFromBash("Process has exited\n");
}

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

    // spawning bash session
    // char* const argv[] = {(char*)"bash", nullptr}; 
    // execve(BASH, argv, environ);
   
    const char* nutshellPath = "/home/lanphgphm/Projects/terminix/cpp/appnutshell"; 
    char* const argv[] = {(char*)"appnutshell", nullptr}; 
    ::execve(nutshellPath, argv, environ);

    // if got to here --> fail to exec bash
    perror("execve");
    ::_exit(EXIT_FAILURE);
    return false;
}

void Ptty::sendSignal(int signal){
    if (m_masterFd <= 0){
        perror("Invalid master file descriptor, cannot send signal");
        return;
    }

    // dynamically get current foreground process
    pid_t fgPid = tcgetpgrp(m_masterFd);
    if (fgPid == -1){
        perror("tcgetpgrp");
        return;
    }

    int sendSignalResult = killpg(fgPid, signal);

    if (sendSignalResult == -1){
        // TODO 1.7: cannot send signals as root?
        perror("Failed to send signal");
    }
    else {
        qDebug("Signal sent successfully.");
    }
}

