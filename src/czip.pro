QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# Set to Qt installation path if Windows
QT_PATH_WIN = C:/Qt/Qt5.14.2/5.14.2/mingw73_64

# SOURCES
SOURCES += \
    main.cpp \
    mainwindow.cpp \
    simplecrypt.cpp

# HEADERS
HEADERS += \
    mainwindow.h \
    simplecrypt.h

# FORMS
FORMS += \
    mainwindow.ui

# INSTALL PATH
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# Platform-specific QCA configuration
win32 {
    # QCA for Windows
    INCLUDEPATH += $(QT_PATH_WIN)/include/Qca-qt5/QtCrypto
    DEPENDPATH  += $(QT_PATH_WIN)/include/Qca-qt5/QtCrypto
    CONFIG(release, debug|release): LIBS += -L"$(QT_PATH_WIN)/lib/" -lqca-qt5
    CONFIG(debug, debug|release):   LIBS += -L"$(QT_PATH_WIN)/lib/" -lqca-qt5
} else:unix {
    # QCA for Linux using pkg-config
    CONFIG += link_pkgconfig
    PKGCONFIG += qca2-qt5
}
