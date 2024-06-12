#include "ptty.h"

#include <QtGlobal> // qDebug
#include <QtDebug> // qDebug overload

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
    // maybe print something to ensure that all are done b4 actually shutting down
}

bool Ptty::pairMasterSlaveFd(){
    // opening master side of pty
    m_masterFd = posix_openpt(O_NOCTTY | O_RDWR);
    if (m_masterFd < 0) {
        perror("posix_openpt");
        return false;
    }

    int grantStatus = grantpt(m_masterFd); // grant slave
    int unlockStatus = unlockpt(m_masterFd); // unlock both master & slave
    if (grantStatus < 0) {
        perror("grantpt");
        ::close(m_masterFd);
        return false;
    }
    if (unlockStatus < 0) {
        perror("unlockpt");
        ::close(m_masterFd);
        return false;
    }

    // opening slave side of pty
    char* slaveFileName = ptsname(m_masterFd);
    if (slaveFileName == NULL) {
        perror("ptsname");
        ::close(m_masterFd);
        return false;
    }

    m_slaveFd = open(slaveFileName, O_NOCTTY | O_RDWR);
    if (m_slaveFd < 0){
        perror("open(slaveFileName)");
        ::close(m_masterFd);
        return false;
    }

    return true;
}

bool Ptty::setupPty(){
    bool pairStatus = pairMasterSlaveFd();
    bool spawnStatus = spawnChildProcess();

    if (pairStatus < 0){
        perror("pairMasterSlaveFd");
        return false;
    }
    if (spawnStatus < 0){
        perror("spawnChildProcess");
        return false;
    }

    return true;
}

void Ptty::executeCommand(QString command){
    if (m_masterFd != -1){
        std::lock_guard<std::mutex> lock(m_writeMutex); // dont need manual unlock
        tcflush(m_masterFd, TCIOFLUSH);
        QByteArray cmd = command.toUtf8() + "\n";
        ssize_t written = ::write(m_masterFd, cmd.data(), cmd.size());

        if (written < 0) {
            perror("write(masterFd)");
        } else if (written < cmd.size()) {
            // in case the entire command cant be written at once
            qDebug("Warning: Not all bytes were written to the terminal.");
        }
    }
    // mutex lock auto release when out of scope
}

void Ptty::readLoop(){
    while(!m_stop){
        ssize_t count = ::read(m_masterFd, resultBuffer, BUFFER_SIZE-1);
        if (count > 0) {
            resultBuffer[count] = '\0';
            emit resultReceivedFromBash(resultBuffer);
        }
        else if (count < 0) {
            // IO error occured ~ unhandled "exit"
            // go inform ScreenController::resultReceivedFromPty() now!
            perror("read");
            break;
        }
        else{
            // count == 0 means child process has closed PTY for some reasons
            perror("PTY closed by child process");
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
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

        // Set the child process as the foreground process group
        // if (tcsetpgrp(m_masterFd, m_pid) == -1) {
        //     perror("tcsetpgrp");
        //     return false;
        // }

        return true;
    }

    // setting child process as session leader
    setsid();

    // child process, only need slaveFd
    ::close(m_masterFd);

    // set slave side of pty as stdin, stdout, stderr
    dup2(m_slaveFd, STDIN_FILENO);
    dup2(m_slaveFd, STDOUT_FILENO);
    dup2(m_slaveFd, STDERR_FILENO);

    // ensure that the child process is set as ITS OWN process group
    // setpgid AFTER setsid() is called
    // if (setpgid(0, 0) == -1) {
    //     perror("setpgid");
    //     exit(EXIT_FAILURE);
    // }

    // spawning bash session
    execl("/bin/bash", "/bin/bash", (char*) NULL);

    // if got to here --> fail to exec bash
    perror("execl(bash)");
    exit(EXIT_FAILURE);
    return false;
}

void Ptty::sendSignal(int signal){
    if (m_masterFd <= 0){
        perror("Invalid master file descriptor, cannot send signal");
        return;
    }

    // // Get the foreground process group ID of the terminal
    // pid_t fg_pgid = tcgetpgrp(m_masterFd);
    // if (fg_pgid == -1) {
    //     perror("tcgetpgrp");
    //     return;
    // }
    // qDebug() << "Sending signal" << signal << "to process group" << fg_pgid;

    // // note: some forum discussions said to not use kill()
    // // i have yet to understand why, the manpage for kill()
    // // specifies usage similar how I am using it in this function
    // int sendSignalResult = kill(-fg_pgid, signal);

    int sendSignalResult = -1; // tmp :)

    if (sendSignalResult == -1){
        perror("Failed to send signal");
    }
    else {
        qDebug("Signal sent successfully.");
    }
}

