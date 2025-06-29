cmake_minimum_required(VERSION 3.16)
project(QuickSoundSwitcher VERSION 1.2.0 LANGUAGES C CXX)

set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Installation directory" FORCE)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(QT_QML_GENERATE_QMLLS_INI ON)
set(CMAKE_DISABLE_FIND_PACKAGE_WrapVulkanHeaders TRUE)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Qml
    Quick
    Widgets
    LinguistTools
    Multimedia
)

qt_standard_project_setup(REQUIRES 6.9)

set(SOURCES
    src/audiomanager.cpp
    src/quicksoundswitcher.cpp
    src/shortcutmanager.cpp
    src/soundpanelbridge.cpp
    src/utils.cpp
    src/main.cpp
    src/mediasessionmanager.cpp
)

set(HEADERS
    include/audiomanager.h
    include/policyconfig.h
    include/quicksoundswitcher.h
    include/shortcutmanager.h
    include/soundpanelbridge.h
    include/utils.h
    include/mediasessionmanager.h
)

qt_add_executable(QuickSoundSwitcher WIN32 MACOSX_BUNDLE
    ${SOURCES}
    ${HEADERS}
    resources/resources.qrc
    resources/appicon.rc
)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/version.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/version.h"
)

set(QML_FILES
    qml/SoundPanel.qml
    qml/SettingsWindow.qml
    qml/KeepAlive.qml
)

set(QML_SINGLETONS
    qml/Singletons/UserSettings.qml
)

set_source_files_properties(${QML_SINGLETONS}
    PROPERTIES QT_QML_SINGLETON_TYPE TRUE
)

qt_add_qml_module(QuickSoundSwitcher
    URI Odizinne.QuickSoundSwitcher
    VERSION 1.0
    QML_FILES ${QML_FILES} ${QML_SINGLETONS}
)

# Avoid collision between executable name and QML module directory (QTBUG-98568).
if(NOT WIN32 AND NOT APPLE)
    set_property(TARGET QuickSoundSwitcher PROPERTY OUTPUT_NAME QuickSoundSwitcher_app)
endif()

target_include_directories(QuickSoundSwitcher PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(QuickSoundSwitcher PRIVATE
    Qt::Core
    Qt::Gui
    Qt::Qml
    Qt::Quick
    Qt::Widgets
    Qt::Multimedia
    gdi32
    winmm
    windowsapp
)

target_compile_definitions(QuickSoundSwitcher PRIVATE
    UNICODE
    WINRT_LEAN_AND_MEAN
)

qt_add_translations(${CMAKE_PROJECT_NAME}
    TS_FILES
        i18n/QuickSoundSwitcher_en.ts
        i18n/QuickSoundSwitcher_fr.ts
        i18n/QuickSoundSwitcher_de.ts
        i18n/QuickSoundSwitcher_ko.ts
        i18n/QuickSoundSwitcher_it.ts
        i18n/QuickSoundSwitcher_zh_CN.ts
    RESOURCE_PREFIX "/i18n"
    LUPDATE_OPTIONS "-no-obsolete -no-ui-lines -locations none"
)

add_dependencies(${CMAKE_PROJECT_NAME} update_translations)

install(TARGETS QuickSoundSwitcher
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_qml_app_script(
    TARGET QuickSoundSwitcher
    OUTPUT_SCRIPT deploy_script
    NO_TRANSLATIONS
    DEPLOY_TOOL_OPTIONS --no-compiler-runtime --no-opengl-sw --no-system-dxc-compiler --no-system-d3d-compiler --skip-plugin-types designer,iconengines,qmllint,generic,networkinformation,help,qmltooling,sqldrivers,tls,qmlls
)
install(SCRIPT ${deploy_script})
