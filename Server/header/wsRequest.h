#ifndef WSREQUEST_H
#define WSREQUEST_H

#include <QObject>
#include <QJsonObject>

class wsRequest
{
public:
    enum class requestType {
        RecordHistory,
        NoType
    };

    wsRequest(const QJsonObject& json_request);
    QJsonObject getRequestInfo() const;
    requestType getRequestType() const;

private:
    requestType req_type;
    QJsonObject req_info;
};

#endif // WSREQUEST_H
