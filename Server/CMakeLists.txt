cmake_minimum_required(VERSION 3.14)

project(WebServer LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS WebSockets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS HttpServer)

add_executable(WebServer
  main.cpp
  httpServer.h httpServer.cpp
  RFIDVerifier.h RFIDVerifier.cpp
  entranceItem.h entranceItem.cpp
  wsRequest.h wsRequest.cpp
  wsResponse.h wsResponse.cpp
  entranceHistory.h entranceHistory.cpp
  wsServer.h wsServer.cpp
)
target_link_libraries(WebServer Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(WebServer Qt${QT_VERSION_MAJOR}::WebSockets)
target_link_libraries(WebServer Qt${QT_VERSION_MAJOR}::HttpServer)

include(GNUInstallDirs)
install(TARGETS WebServer
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
