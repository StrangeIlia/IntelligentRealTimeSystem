QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    calculate_task.cpp \
    create_html_task.cpp \
    dialog_processing.cpp \
    load_files_with_ephemeris_task.cpp \
    load_files_with_message_task.cpp \
    parser_of_observation_data.cpp \
    satellite/calculation.cpp \
    main.cpp \
    main_window.cpp \
    parser_ephemeris.cpp \
    parser_of_navigation_messages.cpp \
    satellite/ephemeris_container.cpp \
    satellite/reciever_position_calculator.cpp \
    satellite/satellite_messages_container.cpp \
    satellite/satellite_position_calculator.cpp

HEADERS += \
    calculate_task.h \
    create_html_task.h \
    dialog_processing.h \
    load_files_with_ephemeris_task.h \
    load_files_with_message_task.h \
    parser_of_observation_data.h \
    satellite/calculation.h \
    satellite/ephemeris.h \
    satellite/ephemeris_container.h \
    satellite/navigation_message.h \
    main_window.h \
    parser_ephemeris.h \
    parser_of_navigation_messages.h \
    satellite/observation_data.h \
    satellite/reciever_position_calculator.h \
    satellite/satellite_messages_container.h \
    satellite/satellite_position_calculator.h

FORMS += \
    dialog_processing.ui \
    main_window.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
