# Monitoring System

## Overview
This part provide a sophisticated graphical user interface (GUI) that allows users to connect to a server via WebSocket, manage session states, and visualize session data. The application handles realtime communication and dynamically updates the GUI with new information from the server.

## Key Components
### Application (cpsapplication.h/cpp)
**Framework Initialization**: Initializes the application's main and history windows and sets global styles.</br>
**WebSocket Communication**: Manages WebSocket connections, handles errors, and processes incoming messages.</br>
This component is crucial for enabling real-time communication between the client application and a server using WebSockets. By establishing a WebSocket connection, the application can send and receive messages in a full-duplex manner, which means that both the server and the client can send data independently of one another at any time during the connection's lifecycle. Here's a detailed breakdown of how WebSocket communication is managed within the application:

**1-Establishing Connections**:<br>
When a user enters the WebSocket URL along with their credentials (username and password) and clicks the connect button, the application invokes the attemptConnection method. This method performs the following actions:

- Connection Check: It first checks if the WebSocket is not already connecting or connected. If it is neither, it initiates a new connection using the URL provided by the user.

- Signal Handling: The application connects various signals from the QWebSocket instance to appropriate slots within the Application class. These signals include:

    - **connected**: Triggered when the WebSocket connection is successfully established.

    - **disconnected**: Triggered when the WebSocket connection is closed, either intentionally or due to connection issues.

    - **textMessageReceived**: Triggered whenever a new text message is received over the WebSocket.
    - **errorOccurred**: Triggered when there is an error with the WebSocket communication.

**2-Handling Errors**:<br>
- Error handling is crucial for maintaining robust communication and providing a good user experience:

    - **Immediate Feedback**: If an error occurs during the WebSocket connection attempt or during communication, the errorOccurred signal is triggered. In response, an anonymous lambda function connected to this signal logs the error and can also be used to notify the user through the GUI.

    - **Error Logging**: The error details are logged using 'qDebug()' which helps in debugging and maintaining logs of communication issues.</br><br>

**3-Processing Incoming Messages**:<br>
The application listens for the textMessageReceived signal. When this signal is emitted:

- **Message Parsing**: Incoming messages are assumed to be in a JSON format. The processMessageReceived method parses these JSON messages.

- **Updating Application State**: Depending on the content of the JSON message, various parts of the application's state or user interface might be updated. For example, if the message contains user details, the application updates these details in the main window. If it contains historical data, it displays this data in the history window.

- **Error Handling in Messages**: If the JSON is invalid or unexpected, the method logs the anomaly for troubleshooting purposes.

**Message Processing**: Interprets JSON-formatted messages and updates the application state accordingly.

### MainWindow (cpsmainwindow.h/cpp)
**UI Setup**: Configures layouts, styles, and signal-slot connections.
**User Interaction**: Handles user inputs for server connection and displays updated user information.</br>
**Dynamic UI Updates**: Reflects changes in the user session status on the UI dynamically.</br>

## Usage
**Connecting to a Server**: Enter the server's WebSocket address along with your username and password, then click the 'اتصال به سرور' button.</br>
**Viewing History**: Once connected, use the 'مشاهده تاریخچه' button to fetch and display historical session data.

## Request and Response Style of the Client Module
### Sample request to show on monitor
```bash
{  
  "username": "exampleUser",
  "date": "dd/mm/yyyy",
  "time": "hh/mm"
}
```
### History part
When the 'مشاهده تاریخچه' button is clicked a request like this sample will send to server:
```bash
{
    "command": "fetch_history"
}
```
server in response will send response like this:
```bash
{
    "history":[
        {
            "username": "exampleUser1",
            "date": "dd/mm/yyyy",
            "time": "hh/mm"
        },
        {
            "username": "exampleUser2",
            "date": "dd/mm/yyyy",
            "time": "hh/mm"
        },
    ]

}
```
