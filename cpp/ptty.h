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

#include <unistd.h> // fork, read, write, exec, dup2, close, setsid
#include <pty.h> // fork
#include <sys/wait.h> // wait
#include <stdlib.h> // exit, calloc
#include <string.h> // strcmp
#include <sys/types.h> // pid_t
#include <fcntl.h> // posix_openpt, open
#include <thread>
#include <mutex>
#include <signal.h>

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
    void resultReceivedFromBash(QString result);

public slots:
    void executeCommand(QString command);

private:
    pid_t m_pid; // pid for bash session
    int m_masterFd;
    int m_slaveFd;
    bool m_stop;
    std::thread *m_readThread;
    std::mutex m_writeMutex;
    static const int BUFFER_SIZE = 4096;
    char resultBuffer[BUFFER_SIZE];

    void readLoop();
    bool setupPty();
    bool pairMasterSlaveFd();
    bool spawnChildProcess();
};

#endif // PTTY_H
