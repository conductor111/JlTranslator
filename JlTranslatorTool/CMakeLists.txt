cmake_minimum_required(VERSION 3.7.0)

project(JlTranslatorTool VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Core)
find_package(Qt5Widgets)
find_package(Qt5WebEngineWidgets)

qt5_wrap_ui(uifiles MainWindow.ui QSettingsDlg.ui)

add_executable(${PROJECT_NAME} WIN32
    main.cpp
    JlWebTranslatorWrapper.cpp
    JlDtdFileProcessor.cpp
    QSettingsDlg.cpp
    QSettingsDlg.ui
    MainWindow.cpp
    MainWindow.ui
    test.cpp
)

target_link_libraries(${PROJECT_NAME} Qt5::Core)
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)
target_link_libraries(${PROJECT_NAME} Qt5::WebEngineWidgets)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

if (WIN32)  
add_custom_command(TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND  $ENV{QT_DIR}/bin/windeployqt.exe ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}
  COMMENT "Executing $ENV{QT_DIR}/bin/windeployqt.exe ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}"
  VERBATIM
)
endif (WIN32)
