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
    fileformat.h \
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
    # Desktop Icon
    RC_FILE = app_icon.rc
} else:macx {
    # application icon
    ICON = ../assets/czip.icns
}

# AURA
AURA_PROJECT_DIR = $$PWD/../external/AURA
AURA_BUILD_DIR   = $$AURA_PROJECT_DIR/build
AURA_INSTALL_DIR = $$AURA_BUILD_DIR/deps_install

aura_build.target = $$AURA_INSTALL_DIR/lib/libAURA.a
QMAKE_EXTRA_TARGETS += aura_build
PRE_TARGETDEPS += $$aura_build.target

win32 {
    msvc {
        CMAKE_GENERATOR = "NMake Makefiles"
        BOTAN_ARGS = # MSVC is Botan's default

        NATIVE_SCRIPT_PATH = $$shell_path($$AURA_PROJECT_DIR/build.bat)
        NATIVE_AURA_BUILD_DIR = $$shell_path($$AURA_BUILD_DIR)
        NATIVE_AURA_PROJECT_DIR = $$shell_path($$AURA_PROJECT_DIR)

        # AURA has a build .bat for msvc
        aura_build.commands = $$NATIVE_SCRIPT_PATH \"$$NATIVE_AURA_BUILD_DIR\" \"$$NATIVE_AURA_PROJECT_DIR\" \"$$CMAKE_GENERATOR\" \"$$BOTAN_ARGS\" -DAURA_DISABLE_TESTS=ON

    } else {
        CMAKE_GENERATOR = "MinGW Makefiles"
        BOTAN_ARGS = --os=mingw;--disable-modules=certstor_system,certstor_system_windows

        # AURA has a build .sh for mingw
        aura_build.commands = sh $$AURA_PROJECT_DIR/build.sh $$AURA_BUILD_DIR $$AURA_PROJECT_DIR \"$$CMAKE_GENERATOR\" \"$$BOTAN_ARGS\"
    }
} else:unix {
    aura_build.commands = \
        cmake -B $$AURA_BUILD_DIR $$AURA_PROJECT_DIR && \
        cmake --build $$AURA_BUILD_DIR --target botan_dependency && \
        cmake --build $$AURA_BUILD_DIR --target aura_library
}

INCLUDEPATH += $$AURA_INSTALL_DIR/include
INCLUDEPATH += $$AURA_INSTALL_DIR/include/botan-2
LIBS += -L$$AURA_INSTALL_DIR/lib

msvc {
    LIBS += AURA.lib botan.lib
} else {
    LIBS += -lAURA -lbotan-2
}
