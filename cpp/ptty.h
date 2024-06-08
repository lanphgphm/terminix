#ifndef PTTY_H
#define PTTY_H

#include <QObject>
#include <QtCore/QMutex>
class QSocketNotifier;
class ScreenController;

class Ptty : public QObject
{
    Q_OBJECT

public:
    explicit Ptty(QObject* parent = nullptr);
    virtual ~Ptty();

signals:
    void resultReceivedFromBash(QString result);

public slots:
    void executeCommand(QString command);

private:
    int sanity = 2;
};

#endif // PTTY_H
