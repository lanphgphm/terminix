#include "screencontroller.h"
#include <QtGlobal> // qDebug

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
    emit resultReadySendToView(result);
}
