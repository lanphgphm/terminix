#include "ptty.h"
#include <QProcess>
#include <QtGlobal> // qDebug

Ptty::Ptty(QObject* parent)
    : QObject(parent)
{
    // constructor
}

Ptty::~Ptty(){
    // destructor
}

void Ptty::executeCommand(QString command){
    QProcess process;
    process.start("bash", QStringList() << "-c" << command);
    process.waitForFinished(-1);

    QString result = process.readAllStandardOutput();
    QString error = process.readAllStandardError();

    if (!error.isEmpty()) result += "\Error:\n" + error;
    emit resultReceivedFromBash(result);
}
