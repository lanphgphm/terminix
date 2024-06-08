#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "cpp/screencontroller.h"
#include "cpp/ptty.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    ScreenController screenController;
    Ptty ptty;
    screenController.setPtty(&ptty);

    // controller forward data direction
    QObject::connect(&screenController,
                     &ScreenController::commandReadySendToPty,
                     &ptty,
                     &Ptty::executeCommand);
    // controller backward data direction
    QObject::connect(&ptty,
                     &Ptty::resultReceivedFromBash,
                     &screenController,
                     &ScreenController::resultReceivedFromPty);

    engine.rootContext()->setContextProperty("screenController", &screenController);

    const QUrl url(QStringLiteral("qrc:/terminix/Main.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
