#include "RFIDVerifier.h"

RFIDVerifier::RFIDVerifier(const QString &tags_file_path_)
    : tags_file_path(tags_file_path_) {

    QFile tags_file(tags_file_path_);

    if (!tags_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Could not open file" << tags_file_path_;
    }

    QByteArray json_tags_file = tags_file.readAll();
    QJsonDocument json_tags_document = QJsonDocument::fromJson(json_tags_file);
    QJsonArray tags_array = json_tags_document.array();

    for (const QJsonValue& tag : tags_array) {
        rfid_tags << tag.toString();
    }
}

bool RFIDVerifier::verify(const entranceItem &entrance_item)
{
    bool isVerified = rfid_tags.contains(entrance_item.getRFID());

    if (isVerified) {
        emit verified(entrance_item);
        qDebug() << "RFID Verified!";
    }
    else {
        emit not_verified(entrance_item);
        qDebug() << "RFID Not Verified!";
    }

    return isVerified;
}
