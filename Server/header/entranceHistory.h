#ifndef ENTRANCEHISTORY_H
#define ENTRANCEHISTORY_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QObject>
#include <Qdebug>
#include <QFile>

#include "entranceItem.h"

class entranceHistory : public QObject
{
    Q_OBJECT
public:
    entranceHistory(const QString& history_file_path_);
    ~entranceHistory();

public Q_SLOTS:
    void prepare_history();
    void record_log(const entranceItem& entrance_log);

Q_SIGNALS:
    void history_logs_collected(const QJsonArray& logs);

private:
    QString history_file_path;
    QList<entranceItem> history_logs;
};

#endif // ENTRANCEHISTORY_H
