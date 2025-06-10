QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# Set to Qt installation path if Windows.
# Can be overridden by passing QT_INSTALL_PATH to qmake from the command line.
# Example: qmake my.pro QT_INSTALL_PATH="C:/Custom/Qt/Path"
!isEmpty(QT_INSTALL_PATH) {
    QT_PATH_WIN = $$QT_INSTALL_PATH
} else {
    QT_PATH_WIN = C:/Qt/5.15.2/msvc2019_64 # Default path inside the Docker container
}

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
	#Desktop Icon
	RC_FILE = app_icon.rc
	
    # QCA for Windows
    INCLUDEPATH += $$QT_PATH_WIN/include/Qca-qt5/QtCrypto
    DEPENDPATH  += $$QT_PATH_WIN/include/Qca-qt5/QtCrypto
    CONFIG(release, debug|release): LIBS += -L$$QT_PATH_WIN/lib/ -lqca-qt5
    CONFIG(debug, debug|release):   LIBS += -L$$QT_PATH_WIN/lib/ -lqca-qt5
} else:unix {
    # QCA for Linux using pkg-config
    CONFIG += link_pkgconfig
    PKGCONFIG += qca2-qt5
}
