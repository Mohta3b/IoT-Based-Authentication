#include <QCoreApplication>
#include <QtCore/QCommandLineParser>
#include <QtCore/QCommandLineOption>
// #include <QtCore/QLoggingCategory>

#include "httpServer.h"
#include "wsServer.h"
#include "RFIDVerifier.h"
#include "entranceHistory.h"

const QString RFID_TAGS_FILE_PATH = "database/RFID_tags.json";
const QString LOG_HISTORY_FILE_PATH = "database/History.json";

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qDebug() << "Starting Web Server";

    auto RFID_verifier = new RFIDVerifier(RFID_TAGS_FILE_PATH);
    auto entrance_history = new entranceHistory(LOG_HISTORY_FILE_PATH);
    auto http_server = new httpServer(8081, RFID_verifier);
    auto web_socket_server = new wsServer(1234);

    QObject::connect(RFID_verifier, &RFIDVerifier::verified, entrance_history, &entranceHistory::record_log);
    QObject::connect(RFID_verifier, &RFIDVerifier::verified, web_socket_server, &wsServer::send_verified_entrance);
    QObject::connect(RFID_verifier, &RFIDVerifier::not_verified, web_socket_server, &wsServer::send_not_verified_entrance);
    QObject::connect(web_socket_server, &wsServer::logs_history_requested, entrance_history, &entranceHistory::prepare_history);
    QObject::connect(entrance_history, &entranceHistory::history_logs_collected, web_socket_server, &wsServer::send_history);

    QObject::connect(&a, &QCoreApplication::aboutToQuit, entrance_history, &entranceHistory::deleteLater);
    QObject::connect(&a, &QCoreApplication::aboutToQuit, web_socket_server, &wsServer::deleteLater);

    QObject::connect(&a, &QCoreApplication::aboutToQuit, [&]() {
        qDebug() << "CPS Server stopped";
    });

    return a.exec();
}
