QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
QMAKE_LFLAGS += -L~/Qt/5.14.2/gcc_64/lib

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
    # QCA for Windows (Qt 5.15.2 MinGW)
    INCLUDEPATH += $$PWD/../../../../Qt/5.15.2/mingw81_64/include/Qca-qt5/QtCrypto
    DEPENDPATH  += $$PWD/../../../../Qt/5.15.2/mingw81_64/include/Qca-qt5/QtCrypto
    CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../Qt/5.15.2/mingw81_64/lib/ -llibqca-qt5
    CONFIG(debug, debug|release):   LIBS += -L$$PWD/../../../../Qt/5.15.2/mingw81_64/lib/ -llibqca-qt5
} else:unix {
    # QCA for Linux using pkg-config
    CONFIG += link_pkgconfig
    PKGCONFIG += qca2-qt5
}

