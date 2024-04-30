#include "entranceItem.h"

entranceItem::entranceItem(const QString &rfid_tag_, const QString &date_, const QString &time_)
    : rfid_tag(rfid_tag_),
      date(date_),
      time(time_) {}

entranceItem::entranceItem(const QString &rfid_tag_)
    : rfid_tag(rfid_tag_),
      date(QDateTime::currentDateTime().toString("MM/dd/yyyy")),
      time(QDateTime::currentDateTime().toString("hh:mm")) {}

entranceItem::entranceItem(const QJsonObject &jsonObject)
    : rfid_tag(jsonObject["rfid_tag"].toString()),
      date(jsonObject["date"].toString()),
      time(jsonObject["time"].toString()) {}

QString entranceItem::getRFID() const
{
    return rfid_tag;
}

QJsonObject entranceItem::toQJsonObject() const
{
    QJsonObject itemJsonObject;

    itemJsonObject["rfid_tag"] = rfid_tag;
    itemJsonObject["date"] = date;
    itemJsonObject["time"] = time;

    return itemJsonObject;
}
