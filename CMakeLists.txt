cmake_minimum_required(VERSION 3.16)

project(Nomad VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 COMPONENTS Core Gui Qml Widgets REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Core)

qt_standard_project_setup(REQUIRES 6.8)


qt_add_executable(appNomad
    main.cpp
    ${PROJECT_SOURCES}
)

qt_add_qml_module(appNomad
    URI Nomad
    QML_FILES
        Main.qml
        SOURCES FileWorker/fileworker.h FileWorker/fileworker.cpp
        SOURCES PathTool/pathtool.h PathTool/pathtool.cpp
        SOURCES Items/item.h Items/item.cpp
        SOURCES Items/itemmanager.h Items/itemmanager.cpp
        SOURCES Data/HashTable/tmap.h Data/HashTable/tmap.cpp
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appNomad PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appNomad
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)
target_link_libraries(appNomad
    PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Widgets
)
target_link_libraries(appNomad PRIVATE Qt6::Core)
target_link_libraries(appNomad PRIVATE Qt6::Core)

include(GNUInstallDirs)
install(TARGETS appNomad
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
