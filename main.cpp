#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "cpp/screencontroller.h"
#include "cpp/ptty.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    qmlRegisterType<ScreenController>("com.terminix", 1, 0, "ScreenController");
    QQmlApplicationEngine engine;

    ScreenController screenController;

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
