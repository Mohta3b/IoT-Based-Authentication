#include "wsResponse.h"

wsResponse::wsResponse(responseType res_type_, const QJsonDocument &res_info_)
    : res_type(res_type_), res_info(res_info_) {}

int wsResponse::getResponseType() const
{
    return static_cast<int>(res_type);
}

QJsonDocument wsResponse::toJsonDocument() const
{
    QJsonObject json_obj;

    if (res_info.isArray()) {
        json_obj["history"] = res_info.array();
    }
    else {
        json_obj["info"] = res_info.object();
    }

    json_obj["response_type"] = static_cast<int>(res_type);

    return QJsonDocument(json_obj);
}
