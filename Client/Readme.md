# Monitoring System

## Overview
This part provide a sophisticated graphical user interface (GUI) that allows users to connect to a server via WebSocket, manage session states, and visualize session data. The application handles realtime communication and dynamically updates the GUI with new information from the server.

## Key Components
### Application (cpsapplication.h/cpp)
**Framework Initialization**: Initializes the application's main and history windows and sets global styles.</br>
**WebSocket Communication**: Manages WebSocket connections, handles errors, and processes incoming messages.</br>
**Message Processing**: Interprets JSON-formatted messages and updates the application state accordingly.

### MainWindow (cpsmainwindow.h/cpp)
**UI Setup**: Configures layouts, styles, and signal-slot connections.
**User Interaction**: Handles user inputs for server connection and displays updated user information.</br>
**Dynamic UI Updates**: Reflects changes in the user session status on the UI dynamically.</br>

## Usage
**Connecting to a Server**: Enter the server's WebSocket address along with your username and password, then click the 'اتصال به سرور' button.</br>
**Viewing History**: Once connected, use the 'مشاهده تاریخچه' button to fetch and display historical session data.