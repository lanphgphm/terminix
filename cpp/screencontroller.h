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
// #include <QFile> // to go to the file

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
        {Qt::Key_Z, SIGTSTP}
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
    QStringList m_commandHistory;
    int m_historyIndex;

    QString processPrompt(const QString& ansiText);
};

#endif // SCREENCONTROLLER_H
