#ifndef CPSAPPLICATION_H
#define CPSAPPLICATION_H

#include <QObject>
#include <QApplication>

#include "cpsmainwindow.h"
#include "cpshistorywindow.h"
#include "cpswindowsapitools.h"
#include <QWebSocket>
#include <QDebug>
#include <QJsonDocument>

namespace CPS {

class Application : public QObject
{
    Q_OBJECT

public:

    explicit Application(QObject *parent = nullptr);
    ~Application();

    void show();

Q_SIGNALS:

private Q_SLOTS:
    void showHistoryWindow();
    void onConnected();
    void onDisconnected();

private: // methods
    void attemptConnection(const QString& address, const QString& username, const QString& password);
    void processMessageReceived(const QString &message);
    void testProcessMessageReceived();

private: // members
    MainWindow    *_window;
    HistoryWindow *_history;
    QWebSocket _webSocket;
};

} // end of CPS

#endif // CPSAPPLICATION_H
