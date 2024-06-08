#ifndef SCREENCONTROLLER_H
#define SCREENCONTROLLER_H

#include <QObject>
#include <QProcess>
#include <QFont>
#include <QString>

class Ptty;

class ScreenController : public QObject
{
    Q_OBJECT

public:
    explicit ScreenController(QObject* parent = nullptr);
    virtual ~ScreenController();

    void setPtty(Ptty* ptty);

signals:
    void resultReadySendToView(QString result);
    void commandReadySendToPty(QString command);

public slots:
    Q_INVOKABLE void commandReceivedFromView(QString command);
    void resultReceivedFromPty(QString result);

private:
    int sanity = 1;
    Ptty* m_ptty;
};

#endif // SCREENCONTROLLER_H
