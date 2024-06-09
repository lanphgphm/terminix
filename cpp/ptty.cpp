#include "ptty.h"

#include <QtGlobal> // qDebug

Ptty::Ptty(QObject* parent)
    : QObject(parent)
{
    // constructor
    setupPty();
}

Ptty::~Ptty(){
    // destructor
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

bool Ptty::spawnChildProcess(){
    char* EV[] = {
        "TERM=xterm-color", "COLORTERM=xterm",
        "COLORFGBG=15;0", "LINES", "COLUMNS", "TERMCAP"
    };

    int N_EV = sizeof(EV) / sizeof(EV[0]);

    m_pid = fork();
    if (m_pid < 0) {
        perror("fork");
        return false;
    }
    else if (m_pid > 0){ // parent process, only need masterFd
        ::close(m_slaveFd);
        return true;
    }

    // child process, only need slaveFd
    setsid();
    ::close(m_masterFd);

    // set slave side of pty as stdin, stdout, stderr
    dup2(m_slaveFd, STDIN_FILENO);
    dup2(m_slaveFd, STDOUT_FILENO);
    dup2(m_slaveFd, STDERR_FILENO);

    // spawning bash session
    execl("/bin/bash", "/bin/bash", (char*) NULL);

    // if got to here --> fail to exec bash
    perror("execl(bash)");
    exit(EXIT_FAILURE);
    return false;
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
        QByteArray cmd = command.toLocal8Bit() + "\n";
        ::write(m_masterFd, cmd.data(), cmd.size());
        char result[1024];
        ssize_t count = ::read(m_masterFd, result, 1024-1);

        if (count > 0) { // something was read from buffer
            result[count] = '\0'; // character not string
            emit resultReceivedFromBash(QString::fromLocal8Bit(result));
        }
        else if (count < 0) {
            perror("read");
        }
        else{
            perror("buffer empty");
        }
    }
}
