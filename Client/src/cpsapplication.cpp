#include "cpsapplication.h"

namespace CPS {

Application::Application(QObject *parent)
    : QObject{parent},
    _window(new MainWindow),
    _history(new HistoryWindow)
{
    setWindowsThemeToDark<MainWindow>(*_window);

    QObject::connect(_window, &MainWindow::connectBtnClicked, this, &Application::attemptConnection);
    QObject::connect(_window, &MainWindow::historyuBtnClicked, this, &Application::showHistoryWindow);
    QObject::connect(&_webSocket, &QWebSocket::textMessageReceived, this, &Application::processMessageReceived);
}

void Application::attemptConnection(const QString& address, const QString& username, const QString& password) {
    connect(&_webSocket, &QWebSocket::connected, this, &Application::onConnected);
    connect(&_webSocket, &QWebSocket::disconnected, this, &Application::onDisconnected);
    connect(&_webSocket, &QWebSocket::textMessageReceived, this, &Application::processMessageReceived);

    connect(&_webSocket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::errorOccurred),
            [=](QAbstractSocket::SocketError errorOccurred){
                qDebug() << "WebSocket Error:" << _webSocket.errorString();
            });

    if (_webSocket.state() != QAbstractSocket::ConnectedState && _webSocket.state() != QAbstractSocket::ConnectingState) {
        _webSocket.open(QUrl(address));
    }
}
void Application::onConnected() {
    qDebug() << "WebSocket connected";
}

void Application::onDisconnected() {
    qDebug() << "WebSocket disconnected";
}

Application::~Application()
{
    delete this->_window;
    delete this->_history;
}

void Application::show()
{
    this->_window->show();
}

void Application::showHistoryWindow()
{
    setWindowsThemeToDark<HistoryWindow>(*_history);
    if (_webSocket.state() == QAbstractSocket::ConnectedState) {
        _webSocket.sendTextMessage(QStringLiteral("{\"request_type\": \"record_history\"}"));
    } else {
        qDebug() << "WebSocket is not connected!";
    }
}

void Application::processMessageReceived(const QString &message) {
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());

    if (!doc.isNull() && doc.isObject()) {
        QJsonObject jsonObject = doc.object();

        if (jsonObject.contains("access") && jsonObject["access"].toString() == "denied") {

            qDebug() << "Access Denied!";
            _window->showMessage("Access Denied");

        } else if (jsonObject.contains("info")) {
            QJsonObject jsonData = jsonObject["info"].toObject();
            QString rfid = jsonData["rfid_tag"].toString();
            QString date = jsonData["date"].toString();
            QString time = jsonData["time"].toString();
            _window->updateUserDetails(rfid, date, time);

        } else if (jsonObject.contains("history") && jsonObject["history"].isArray()) {

            QJsonArray historyData = jsonObject["history"].toArray();
            qDebug() << "history recieved:" << historyData;
            _history->show(historyData);
            _history->show(historyData);

        } else {
            qDebug() << "Unexpected JSON format received"<< jsonObject;
        }
    } else {
        qDebug() << "Invalid or non-JSON message received:" << message;
    }
}

void Application::testProcessMessageReceived() {
    QString mockMessage = R"({"username": "cps", "date": "30/4/2024", "time": "12:00"})";
    processMessageReceived(mockMessage);
}

} // end of CPS
