#-------------------------------------------------
#
# Project created by QtCreator 2018-04-01T12:10:45
#
#-------------------------------------------------

QT       += core gui printsupport sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Client
TEMPLATE = app
QTPLUGIN += qtaccessiblewidgets
# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    SettingsDialog.cpp \
    ProxyModel.cpp \
    PriceRules.cpp \
    DateDelegate.cpp \
    SubscriptionDelegate.cpp

HEADERS += \
        mainwindow.h \
    SettingsDialog.h \
    ProxyModel.h \
    PriceRules.h \
    DateDelegate.h \
    SubscriptionDelegate.h

FORMS += \
        mainwindow.ui \
    SettingsDialog.ui \
    PriceRules.ui

RESOURCES += \
    clientresources.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Core/release/ -lCore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Core/debug/ -lCore
else:unix: LIBS += -L$$OUT_PWD/../Core/ -lCore

INCLUDEPATH += $$PWD/../Core
DEPENDPATH += $$PWD/../Core
