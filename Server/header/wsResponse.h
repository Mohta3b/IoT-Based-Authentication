#ifndef WSRESPONSE_H
#define WSRESPONSE_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QObject>

class wsResponse
{
public:
    enum class responseType {
        Ok = 200,
        BadRequest = 400,
        Unauthorized = 401,
    };

    wsResponse(responseType res_type_, const QJsonDocument& res_info_ = QJsonDocument());
    int getResponseType() const;
    QJsonDocument toJsonDocument() const;

private:
    responseType res_type;
    QJsonDocument res_info;
};

#endif // WSRESPONSE_H
