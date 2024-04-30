#ifndef ENTRANCEITEM_H
#define ENTRANCEITEM_H

#include <QJsonObject>
#include <QDateTime>
#include <QString>

class entranceItem
{
public:
    entranceItem(const QString& rfid_tag_, const QString& date_, const QString& time_);
    entranceItem(const QString& rfid_tag_);
    entranceItem(const QJsonObject& jsonObject);
    QString getRFID() const;
    QJsonObject toQJsonObject() const;

private:
    QString rfid_tag;
    QString date;
    QString time;
};

#endif // ENTRANCEITEM_H
