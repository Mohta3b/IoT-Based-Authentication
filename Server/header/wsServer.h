#ifndef WSSERVER_H
#define WSSERVER_H

#include "QtWebSockets/qwebsocket.h"
#include "QtWebSockets/qwebsocketserver.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>
#include <QObject>

#include "entranceItem.h"
#include "wsResponse.h"
#include "wsRequest.h"

class wsServer : public QObject
{
    Q_OBJECT
public:
    wsServer(int ws_port_, QObject* parent = nullptr);

public Q_SLOTS:
    void set_up_connection();
    void disconnect_client();
    void handle_message(const QString& message);
    void send_message(const wsResponse& ws_response);
    void send_history(const QJsonArray& history_logs);
    void send_verified_entrance(const entranceItem& entrance_item);
    void send_not_verified_entrance(const entranceItem& entrance_item);

Q_SIGNALS:
    void logs_history_requested();

private:
    QWebSocketServer* server;
    QWebSocket* monitoring_client;
    int ws_port;
    bool client_has_access;
};

#endif // WSSERVER_H
