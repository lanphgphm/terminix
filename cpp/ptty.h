/* NOTES: Targeting high-POSIX compliant systems for high availability
 * and portability. From the man page of pty:
    Since  Linux 2.6.4, BSD-style pseudoterminals are considered
    deprecated: support can be  disabled  when  building the kernel
    by  disabling the CONFIG_LEGACY_PTYS option.

    (Starting with Linux 2.6.30, that option is  disabled  by default
    in the mainline kernel.)  UNIX 98 pseudoterminals should be used
    in new applications.
 */

#ifndef PTTY_H
#define PTTY_H

#include <unistd.h> // fork, read, write, exec, dup2, close, setsid, setpgid
#include <cstring>  // strerror
#include <cstdlib>  // environ
#include <pty.h> // ioctl
#include <stdlib.h> // exit
#include <sys/types.h> // pid_t, ssize_t
#include <sys/select.h> // select 
#include <fcntl.h> // posix_openpt
#include <thread> // thread, this_thread, detach, joinable, join, sleep_for

#include <mutex> // lock_guard, mutex,lock
#include <signal.h> // killpg

#include <QObject>

class Ptty : public QObject
{
    Q_OBJECT

public:
    explicit Ptty(QObject* parent = nullptr);
    virtual ~Ptty();

    void start();
    void stop();
    void sendSignal(int signal);

signals:
    void resultReceivedFromShell(QString result);

public slots:
    void executeCommand(QString command);

private:
    pid_t m_pid; // pid for shell session
    int m_masterFd;
    int m_slaveFd;
    bool m_stop;
    std::thread *m_readThread;
    std::mutex m_writeMutex;
    static const int OUTPUT_BUFFER_SIZE = 8192;
    char resultBuffer[OUTPUT_BUFFER_SIZE];

    void readLoop();
    bool setupPty();
    bool pairMasterSlaveFd();
    bool spawnChildProcess();
};

#endif // PTTY_H
