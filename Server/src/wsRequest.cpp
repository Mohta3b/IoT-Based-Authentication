#include "wsRequest.h"

wsRequest::wsRequest(const QJsonObject &json_request)
{
    QString req_type_ = json_request["request_type"].toString();

    if (json_request.contains("info")) {
        req_info = json_request["info"].toObject();
    }

    if (req_type_ == "record_history") {
        req_type = requestType::RecordHistory;
    }
    else {
        req_type = requestType::NoType;
    }
}

QJsonObject wsRequest::getRequestInfo() const
{
    return req_info;
}

wsRequest::requestType wsRequest::getRequestType() const
{
    return req_type;
}
