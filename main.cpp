#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QLoggingCategory>
#include <QQmlContext>

#include "bluetootherror.h"
#include "devicefind.h"
#include "devicehandle.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    Bluetootherror connectionHandler;
    DeviceHandle deviceHandler;
    Devicefind deviceFinder(&deviceHandler);

    qmlRegisterUncreatableType<DeviceHandle>("Shared", 1, 0, "AddressType", "Enum is not a type");


    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("connectionHandler", &connectionHandler);
    engine.rootContext()->setContextProperty("deviceFinder", &deviceFinder);
    engine.rootContext()->setContextProperty("deviceHandler", &deviceHandler);

    const QUrl url(u"qrc:/Metal_Detector/main.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
