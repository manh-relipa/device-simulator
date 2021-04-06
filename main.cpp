#include "socketmanager.h"
#include "dtusimulator.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <signal.h>
static int exit_flag = 0;

static void sigexit(int sig)
{
    qInfo() << "Terminate signal";
    Q_UNUSED(sig);
    exit_flag = 1;
    QCoreApplication::exit(exit_flag);
}

static int setup_unix_signal_handlers()
{
    signal(SIGTERM, sigexit); // catch kill signal
    signal(SIGHUP, sigexit);  // catch hang up signal
    signal(SIGQUIT, sigexit); // catch quit signal
    signal(SIGINT, sigexit);  // catch a CTRL-c signal

    return 0;
}

int main(int argc, char *argv[])
{
    setup_unix_signal_handlers();
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);
    auto socketManger = std::make_shared<SocketManager>();
    auto dtuSimulator = std::make_shared<DtuSimulator>();

    socketManger->addMessageProvider(dtuSimulator);
    socketManger->addHandler(dtuSimulator);

    socketManger->setupSocket();


    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("dtuSimulator", dtuSimulator.get());
    engine.rootContext()->setContextProperty("socketManager", socketManger.get());

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
