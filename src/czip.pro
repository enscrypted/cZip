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
} else:macx {
    # This block provides two ways to find the QCA library:
    # 1. (For the build script) A QCA_PREFIX variable passed on the command line.
    # 2. (For Qt Creator) A fallback that checks for a standard Homebrew installation.

    # First, check if the prefix is being passed by our build script.
    !isEmpty(QCA_PREFIX) {
        message("Using QCA path from build script: $$QCA_PREFIX")
        # For macOS frameworks, the headers are in a different location
        INCLUDEPATH += $$QCA_PREFIX/lib/qca-qt5.framework/Headers
        # And we need to link it as a framework, not a standard library
        LIBS += -F$$QCA_PREFIX/lib -framework qca-qt5
    } else {
        # If not, this is likely a Qt Creator build. Try to find Homebrew's QCA.
        message("QCA_PREFIX not set, searching for a Homebrew installation...")

        # Path for Apple Silicon Homebrew
        if(exists(/opt/homebrew/opt/qca)) {
            message("Found QCA at Apple Silicon Homebrew path.")
            QCA_HOMEBREW_PREFIX = /opt/homebrew/opt/qca
        # Path for Intel Homebrew
        } else: if(exists(/usr/local/opt/qca)) {
            message("Found QCA at Intel Homebrew path.")
            QCA_HOMEBREW_PREFIX = /usr/local/opt/qca
        }

        # If we found a Homebrew path, use it.
        !isEmpty(QCA_HOMEBREW_PREFIX) {
            INCLUDEPATH += $$QCA_HOMEBREW_PREFIX/include/Qca-qt5
            LIBS += -L$$QCA_HOMEBREW_PREFIX/lib -lqca-qt5
        } else {
            warning("Could not find QCA. Build will likely fail. Please install via 'brew install qca' or use the build script.")
        }
    }

    # Set the application icon
    ICON = ../assets/czip.icns
} else:unix {
    # QCA for Linux using pkg-config
    CONFIG += link_pkgconfig
    PKGCONFIG += qca2-qt5
}
