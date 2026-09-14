#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QApplication>
#include "PathTool/pathtool.h"
#include "Items/itemmanager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;

    QPathTool PathTool;
    engine.rootContext()->setContextProperty("PathTool", &PathTool);

    QItemManager ItemManager;
    engine.rootContext()->setContextProperty("ItemManager", &ItemManager);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("Nomad", "Main");

    return app.exec();
}
