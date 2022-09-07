QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += \
    c++17
    crypto

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    simplecrypt.cpp
    simplecrypt.cpp

HEADERS += \
    mainwindow.h \
    simplecrypt.h
    simplecrypt.cpp

FORMS += \
    mainwindow.ui


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target




win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../Qt/5.15.2/mingw81_64/lib/ -llibqca-qt5.dll
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../Qt/5.15.2/mingw81_64/lib/ -llibqca-qt5.dll
else:unix: LIBS += -L$$PWD/../../../../Qt/5.15.2/mingw81_64/lib/ -llibqca-qt5.dll

INCLUDEPATH += $$PWD/../../../../Qt/5.15.2/mingw81_64/include/Qca-qt5/QtCrypto
DEPENDPATH += $$PWD/../../../../Qt/5.15.2/mingw81_64/include/Qca-qt5/QtCrypto
