cmake_minimum_required(VERSION 3.0.0)

project(JlTranslatorTool VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Core)
find_package(Qt5Widgets)
find_package(Qt5WebEngineWidgets)

qt5_wrap_ui(uifiles MainWindow.ui QSettingsDlg.ui)

add_executable(${PROJECT_NAME}
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

#add_custom_command(TARGET ${PROJECT_NAME}
#  # Run after all other rules within the target have been executed
# POST_BUILD
#  COMMAND cmake -E copy ${CMAKE_CURRENT_LIST_DIR}/focus_log ${CMAKE_CURRENT_LIST_DIR}/focus_log_1 ${CMAKE_CURRENT_LIST_DIR}/focus_log_2 ${CMAKE_BINARY_DIR}/${CMAKE_CFG_INTDIR}
#  COMMENT "Copying ${CMAKE_CURRENT_LIST_DIR}/focus_log ${CMAKE_CURRENT_LIST_DIR}/focus_log_1 ${CMAKE_CURRENT_LIST_DIR}/focus_log_2 ${CMAKE_BINARY_DIR}/${CMAKE_CFG_INTDIR}"
#  VERBATIM
#)
