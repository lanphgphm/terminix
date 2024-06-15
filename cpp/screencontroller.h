#ifndef SCREENCONTROLLER_H
#define SCREENCONTROLLER_H

#include "cpp/ptty.h"

// to read and write /root/.bash_history
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cerrno>
#include <cstring>

#include <QObject>
#include <QProcess>
#include <QFont>
#include <QString>
#include <QRegularExpression>
#include <QMap>
#include <QKeyEvent>
#include <QFile> // to go to the file
#include <QTextStream> // to read the .bash_history file
#include<QDir> // find home of each user, including root

class Ptty;

class ScreenController : public QObject
{
    Q_OBJECT

public:
    explicit ScreenController(QObject* parent = nullptr);
    virtual ~ScreenController();

    QString ansiToHtml(const QString& ansiText);

signals:
    void resultReadySendToView(QString result);
    void commandReadySendToPty(QString command);
    void terminalSessionEnded();
    void showCommand(QString command);

public slots:
    Q_INVOKABLE void commandReceivedFromView(QString command);
    Q_INVOKABLE void handleControlKeyPress(Qt::Key key);
    void resultReceivedFromPty(QString result);
    Q_INVOKABLE void commandHistoryUp();
    Q_INVOKABLE void commandHistoryDown();
    Q_INVOKABLE void logCommand(QString command);

private:
    Ptty* m_ptty;
    QMap<Qt::Key, int> keySignalMap = {
        {Qt::Key_C, SIGINT},
        {Qt::Key_Z, SIGTSTP},
        {Qt::Key_Backslash, SIGQUIT},
        {Qt::Key_S, SIGSTOP},
        {Qt::Key_Q, SIGCONT},
    };

    QMap<int, QString> colorMap = {
        {30, "#000000"}, // black
        {31, "#a54242"}, // red
        {32, "#88d22f"}, // green
        {33, "#e8d366"}, // yellow
        {34, "#38cbca"}, // blue
        {35, "#cd7db7"}, // magenta
        {36, "#b294bb"}, // cyan
        {37, "#d5d5d5"} // white
    };

    bool isRoot = false;
    // note: if parse ansi text correctly, bash will handle history itself
    // without having the app to load the command history in to a list like this
    // this is a work-around, but be mindful that it takes up extra memory
    QStringList m_commandHistory;
    int m_historyIndex;
    void loadBashCommandHistoryFile();

    QStringList m_rootCommandHistory;
    int m_rootHistoryIndex;
    void loadRootBashCommandHistoryFile();

    QString processPrompt(const QString& ansiText);
    QString removeAnsi(const QString& ansiTest);

};

#endif // SCREENCONTROLLER_H
