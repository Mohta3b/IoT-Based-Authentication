#include "entranceHistory.h"

entranceHistory::entranceHistory(const QString &history_file_path_)
{
    history_file_path = history_file_path_;

    history_logs.clear();

    QFile logs_file(history_file_path_);
    if (!logs_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Could not open file" << history_file_path_;
    }

    QByteArray json_logs_file = logs_file.readAll();
    QJsonDocument json_logs_document = QJsonDocument::fromJson(json_logs_file);
    QJsonArray logs_array = json_logs_document.array();

    for (const auto& log : logs_array) {
        entranceItem entrance_log(log.toObject());
        history_logs << entrance_log;
    }
}

entranceHistory::~entranceHistory()
{
    qDebug() << "entranceHistory Destructor Called!";

    QJsonArray logs_array;

    for (const auto& log : history_logs) {
        logs_array << log.toQJsonObject();
    }

    QJsonDocument logs_document(logs_array);
    QByteArray json_logs = logs_document.toJson();

    QFile logs_file(history_file_path);

    if (!logs_file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Could not create file" << history_file_path;
    }

    logs_file.write(json_logs);

    qDebug() << "History Saved!";
}

void entranceHistory::prepare_history()
{
    QJsonArray history_array;

    for (const auto& log : history_logs) {
        history_array << log.toQJsonObject();
    }

    emit history_logs_collected(history_array);
}

void entranceHistory::record_log(const entranceItem &entrance_log)
{
    history_logs << entrance_log;
}
