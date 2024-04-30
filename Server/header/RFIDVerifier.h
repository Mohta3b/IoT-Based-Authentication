#ifndef RFIDVERIFIER_H
#define RFIDVERIFIER_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QObject>
#include <QString>
#include <Qdebug>
#include <QFile>

#include "entranceItem.h"

class RFIDVerifier : public QObject
{
    Q_OBJECT
public:
    RFIDVerifier(const QString& tags_file_path_);
    bool verify(const entranceItem& entrance_item);

private:
    QString tags_file_path;
    QList<QString> rfid_tags;

Q_SIGNALS:
    void verified(entranceItem entrance_item);
    void not_verified(entranceItem entrance_item);
};

#endif // RFIDVERIFIER_H
