cmake_minimum_required(VERSION 3.5)

project(Barbarian LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.


find_package(Qt5 COMPONENTS Widgets Network REQUIRED)

  add_executable(Barbarian
    main.cpp
    barbarian.cpp
    barbarian.h
    barbarian.ui
  )

target_link_libraries(Barbarian PRIVATE Qt5::Widgets Qt5::Network)
