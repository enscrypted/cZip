QT      += core gui
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

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    simplecrypt.cpp

HEADERS += \
    mainwindow.h \
    simplecrypt.h

FORMS += \
    mainwindow.ui

# INSTALL PATH
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# platform-specific QCA configuration
win32 {
    # desktop Icon
    RC_FILE = app_icon.rc

    # QCA for Windows
    INCLUDEPATH += $$QT_PATH_WIN/include/Qca-qt5/QtCrypto
    DEPENDPATH  += $$QT_PATH_WIN/include/Qca-qt5/QtCrypto
    CONFIG(release, debug|release): LIBS += -L$$QT_PATH_WIN/lib/ -lqca-qt5
    CONFIG(debug, debug|release):  LIBS += -L$$QT_PATH_WIN/lib/ -lqca-qt5
} else:macx {
    # this block provides two ways to find the QCA library:
    # 1. a QCA_PREFIX variable passed on the command line.
    # 2. a fallback that checks for a standard Homebrew installation.
    !isEmpty(QCA_PREFIX) {
        message("Using QCA path from build script: $$QCA_PREFIX")
        INCLUDEPATH += $$QCA_PREFIX/lib/qca-qt5.framework/Headers
        LIBS += -F$$QCA_PREFIX/lib -framework qca-qt5
    } else {
        message("QCA_PREFIX not set, searching for a Homebrew installation...")

        # path for Apple Silicon Homebrew
        if(exists(/opt/homebrew/opt/qca)) {
            message("Found QCA at Apple Silicon Homebrew path.")
            QCA_HOMEBREW_PREFIX = /opt/homebrew/opt/qca
        # path for Intel Homebrew
        } else: if(exists(/usr/local/opt/qca)) {
            message("Found QCA at Intel Homebrew path.")
            QCA_HOMEBREW_PREFIX = /usr/local/opt/qca
        }

        !isEmpty(QCA_HOMEBREW_PREFIX) {
            INCLUDEPATH += $$QCA_HOMEBREW_PREFIX/include/Qca-qt5
            LIBS += -L$$QCA_HOMEBREW_PREFIX/lib -lqca-qt5
        } else {
            warning("Could not find QCA. Build will likely fail. Please install via 'brew install qca' or use the build script.")
        }
    }

    # application icon
    ICON = ../assets/czip.icns
} else:unix {
    # QCA for Linux using pkg-config
    CONFIG += link_pkgconfig
    PKGCONFIG += qca2-qt5
}

# Define AURA paths
AURA_SOURCES_DIR = $$PWD/../external/AURA
AURA_BUILD_DIR   = $$AURA_SOURCES_DIR/build
AURA_INSTALL_DIR = $$AURA_BUILD_DIR/deps_install 

aura_build.target = $$AURA_INSTALL_DIR/lib/libAURA.a

# run the entire AURA super-build
aura_build.commands = \
    cmake -B $$AURA_BUILD_DIR $$AURA_SOURCES_DIR && \
    cmake --build $$AURA_BUILD_DIR

aura_build.depends = $$AURA_SOURCES_DIR/src/AURA.cpp
QMAKE_EXTRA_TARGETS += aura_build

# wait for AURA to be built
PRE_TARGETDEPS += $$aura_build.target

INCLUDEPATH += $$AURA_INSTALL_DIR/include
LIBS += -L$$AURA_INSTALL_DIR/lib -lAURA -lbotan-2
