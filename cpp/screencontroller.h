#ifndef SCREENCONTROLLER_H
#define SCREENCONTROLLER_H

#include "cpp/ptty.h"

#include <QObject>
#include <QProcess>
#include <QFont>
#include <QString>
#include <QRegularExpression>
#include <QMap>
#include <QKeyEvent>

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

public slots:
    Q_INVOKABLE void commandReceivedFromView(QString command);
    Q_INVOKABLE void handleControlKeyPress(Qt::Key key);
    void resultReceivedFromPty(QString result);

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
        {30, "black"},
        {31, "red"},
        {32, "green"},
        {33, "yellow"},
        {34, "blue"},
        {35, "magenta"},
        {36, "cyan"},
        {37, "white"}
    };
};

#endif // SCREENCONTROLLER_H
