#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QHttpServerResponse>
#include <QHttpServerRequest>
#include <QJsonDocument>
#include <QHttpServer>
#include <QJsonObject>
#include <QJsonArray>
#include <QObject>

#include "RFIDVerifier.h"

class httpServer : public QObject
{
    Q_OBJECT
public:
    httpServer(int port_, RFIDVerifier* RFID_verifier_, QObject *parent = nullptr);
    ~httpServer();
    void start_server();
    void stop_server();
    QHttpServerResponse request_handler(const QHttpServerRequest& request);

private:
    int port;
    QHttpServer* server;
    RFIDVerifier* RFID_verifier;
};

#endif // HTTPSERVER_H
