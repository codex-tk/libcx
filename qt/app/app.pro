#-------------------------------------------------
#
# Project created by QtCreator 2017-08-20T20:52:25
#
#-------------------------------------------------

QT       += core gui


INCLUDEPATH +=  ./../../

QMAKE_CXXFLAGS += /std:c++17

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = app
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
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
    qtconvinience.cpp \
    dialog.cpp \
    binarysampledialog.cpp \
    context.cpp \
    histogramdialog.cpp \
    fftdialog.cpp \
    kmeandialog.cpp \
    mouseeventsignaler.cpp

HEADERS += \
        mainwindow.hpp \
    qtconvinience.hpp \
    dialog.h \
    binarysampledialog.h \
    app.hpp \
    context.h \
    histogramdialog.hpp \
    fftdialog.hpp \
    kmeandialog.h \
    mouseeventsignaler.h

FORMS += \
        mainwindow.ui \
    dialog.ui \
    binarysampledialog.ui \
    histogramdialog.ui \
    fftdialog.ui \
    kmeandialog.ui

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../cx/release/ -lcx
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../cx/debug/ -lcx
else:unix: LIBS += -L$$OUT_PWD/../cx/ -lcx

INCLUDEPATH += $$PWD/../cx
DEPENDPATH += $$PWD/../cx

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../cx/release/libcx.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../cx/debug/libcx.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../cx/release/cx.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../cx/debug/cx.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../cx/libcx.a

RESOURCES += \
    res.qrc
