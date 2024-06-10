#include "screencontroller.h"
#include <QtGlobal> // qDebug
#include <QRegularExpression>
#include <QMap>

ScreenController::ScreenController(QObject* parent)
    : QObject(parent)
{
    // constructor
}

ScreenController::~ScreenController() {
    // destructor
}

void ScreenController::setPtty(Ptty* ptty){
    m_ptty = ptty;
}

void ScreenController::commandReceivedFromView(QString command){
    emit commandReadySendToPty(command);
}

void ScreenController::resultReceivedFromPty(QString result){
    // QString htmlResult = ansiToHtml(result);
    // emit resultReadySendToView(htmlResult);

    emit resultReadySendToView(result); // baseline ugly
}

QString ScreenController::ansiToHtml(const QString ansiText){
    QString htmlText = ansiText;

    //------------------------------------------------
    // do convert
    //------------------------------------------------

    return htmlText;
}
