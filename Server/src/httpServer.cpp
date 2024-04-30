#include "httpServer.h"

httpServer::httpServer(int port_, RFIDVerifier *RFID_verifier_, QObject *parent)
    : QObject{parent}, port{port_}, server{new QHttpServer()}, RFID_verifier(RFID_verifier_) {

    server->route("/verification", [this](const QHttpServerRequest& request) {
        return this->request_handler(request);
    });

    start_server();
}

httpServer::~httpServer()
{
    server->deleteLater();
}

void httpServer::start_server()
{
    if (server->listen(QHostAddress::Any, port)) {
        qInfo() << "HTTP Server Listening On Port:" << port;
    } else {
        qFatal("Unable Oo Start HTTP Server On Port: %d", port);
        return;
    }
}

void httpServer::stop_server()
{
    server->disconnect();
}

QHttpServerResponse httpServer::request_handler(const QHttpServerRequest &request)
{
    QByteArray json_array = request.body();
    QJsonDocument json_doc = QJsonDocument::fromJson(json_array);

    if (!json_doc.isObject()) {
        return QHttpServerResponse(QByteArray("JSON is not an object"), QHttpServerResponse::StatusCode::BadRequest);
    }

    QJsonObject json_obj = json_doc.object();
    if (!json_obj.contains("rfid") || !json_obj["rfid"].isString()) {
        return QHttpServerResponse(QByteArray("JSON object does not contain a RFID string"), QHttpServerResponse::StatusCode::BadRequest);
    }

    QString rfid_tag = json_obj["rfid"].toString();
    entranceItem entrance_item(rfid_tag);
    QJsonDocument response_json_doc(entrance_item.toQJsonObject());
    QByteArray response_json_array = response_json_doc.toJson();

    bool rfid_verified = RFID_verifier->verify(entrance_item);

    if (rfid_verified) {
        return QHttpServerResponse(response_json_array, QHttpServerResponse::StatusCode::Ok);
    }
    else {
        return QHttpServerResponse(response_json_array, QHttpServerResponse::StatusCode::Unauthorized);
    }
}
