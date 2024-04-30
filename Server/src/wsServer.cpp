#include "wsServer.h"

wsServer::wsServer(int ws_port_, QObject *parent)
    : QObject(parent), ws_port(ws_port_) {

    client_has_access = true;
    server = new QWebSocketServer("Web Socket Server", QWebSocketServer::NonSecureMode, this);
    monitoring_client = nullptr;

    if (!server->listen(QHostAddress::Any, ws_port)) {
        qFatal("Web Socket Server Failed To Listen On Port: %d", ws_port);
    }
    else {
        qInfo() << "Web Socket Server Listening On Port:" << ws_port;
    }

    connect(server, &QWebSocketServer::newConnection, this, &wsServer::set_up_connection);
}

void wsServer::set_up_connection()
{
    client_has_access = true;
    monitoring_client = server->nextPendingConnection();

    connect(monitoring_client, &QWebSocket::textMessageReceived, this, &wsServer::handle_message);
    connect(monitoring_client, &QWebSocket::disconnected, this, &wsServer::disconnect_client);

    qInfo() << "Monitoring Client Connected!";
}

void wsServer::disconnect_client()
{
    monitoring_client->deleteLater();

    qInfo() << "Monitoring Client Disconnected!";
}

void wsServer::handle_message(const QString &message)
{
    QJsonObject json_message = QJsonDocument::fromJson(message.toUtf8()).object();
    wsRequest request(json_message);

    if (request.getRequestType() == wsRequest::requestType::RecordHistory) {
        if (client_has_access) {
            emit logs_history_requested();
            qDebug() << "Logs History Requested!";
            return;
        }
        else {
            send_message(wsResponse(wsResponse::responseType::Unauthorized));
            qDebug() << "Not Verified User Doesn't Have Access To Logs History!";
        }
    }
    else {
        send_message(wsResponse(wsResponse::responseType::BadRequest));
        qDebug() << "Request With Unknown Type Received!";
    }
}

void wsServer::send_message(const wsResponse &ws_response)
{
    if (monitoring_client) {
        monitoring_client->sendTextMessage(ws_response.toJsonDocument().toJson(QJsonDocument::Compact));
    } else {
        qWarning() << "Monitoring Client Not Found!";
    }
}

void wsServer::send_history(const QJsonArray &history_logs)
{
    wsResponse ws_response(wsResponse::responseType::Ok, QJsonDocument(history_logs));
    send_message(ws_response);
    qDebug() << "Logs History Sent!" << history_logs;
}

void wsServer::send_verified_entrance(const entranceItem &entrance_item)
{
    wsResponse ws_response(wsResponse::responseType::Ok, QJsonDocument(entrance_item.toQJsonObject()));
    send_message(ws_response);

    qDebug() << "Verified Entrance Sent - User With RFID: " << entrance_item.getRFID();
}

void wsServer::send_not_verified_entrance(const entranceItem &entrance_item)
{
    wsResponse ws_response(wsResponse::responseType::Unauthorized, QJsonDocument(entrance_item.toQJsonObject()));
    send_message(ws_response);

    qDebug() << "Not Verified Entrance Sent - User With RFID: " << entrance_item.getRFID();
}
