/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QFrame *encryptFrame;
    QCheckBox *encrypt;
    QCheckBox *decrypt;
    QFrame *cryptOpts;
    QPushButton *aesGen;
    QLineEdit *simpKey;
    QPushButton *simpGen;
    QRadioButton *simp;
    QLineEdit *aesKey;
    QRadioButton *aes;
    QFrame *concealFrame;
    QCheckBox *conceal;
    QCheckBox *reveal;
    QFrame *concOpts;
    QFrame *slctImgFrame;
    QLineEdit *slctImgTxt;
    QRadioButton *genImg;
    QRadioButton *slctImg;
    QPushButton *slctImgBrowse;
    QLineEdit *outImgName;
    QPushButton *seededGen;
    QLineEdit *seededKey;
    QFrame *frame_2;
    QRadioButton *seeded;
    QRadioButton *lsb;
    QFrame *universalFrame;
    QPushButton *start;
    QFrame *frame;
    QCheckBox *compress;
    QCheckBox *decompress;
    QPushButton *outFileBrowse;
    QLineEdit *outFile;
    QLineEdit *inFile;
    QPushButton *inFileBrowse;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(927, 628);
        MainWindow->setAutoFillBackground(false);
        MainWindow->setStyleSheet(QString::fromUtf8("QMainWindow{background-color: #383838; }"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        centralwidget->setStyleSheet(QString::fromUtf8("QWidget{background-color: #383838; color: white;}\n"
"QRadioButton::indicator::unchecked{ border: 1px solid darkgray; border-radius: 8px; background-color: white; }\n"
"\n"
"QRadioButton::indicator::checked{ border: 1px solid darkgray; border-radius: 8px; background-color: black; }\n"
"\n"
"QCheckBox::indicator::unchecked{ border: 1px solid darkgray;; background-color: white; }\n"
"\n"
"QCheckBox::indicator::checked{ border: 1px solid darkgray;; background-color: black; }"));
        encryptFrame = new QFrame(centralwidget);
        encryptFrame->setObjectName(QString::fromUtf8("encryptFrame"));
        encryptFrame->setGeometry(QRect(320, 0, 341, 461));
        encryptFrame->setStyleSheet(QString::fromUtf8("QFrame {border: none;}\n"
"QPushButton {background-color: #b5b5b5;color:  #383838;}\n"
"QLineEdit {background-color: #b5b5b5;color:  #383838;}"));
        encryptFrame->setFrameShape(QFrame::StyledPanel);
        encryptFrame->setFrameShadow(QFrame::Raised);
        encrypt = new QCheckBox(encryptFrame);
        encrypt->setObjectName(QString::fromUtf8("encrypt"));
        encrypt->setGeometry(QRect(40, 30, 171, 64));
        QFont font;
        font.setFamily(QString::fromUtf8("SansSerif"));
        encrypt->setFont(font);
        encrypt->setStyleSheet(QString::fromUtf8("QCheckBox::indicator { width:20px; height: 20px;}\n"
"\n"
"QCheckBox { font-size: 30px; font-style: bold;}"));
        encrypt->setIconSize(QSize(16, 16));
        encrypt->setTristate(false);
        decrypt = new QCheckBox(encryptFrame);
        decrypt->setObjectName(QString::fromUtf8("decrypt"));
        decrypt->setGeometry(QRect(50, 100, 171, 41));
        decrypt->setFont(font);
        decrypt->setStyleSheet(QString::fromUtf8("QCheckBox::indicator { width:15px; height: 15px;}\n"
"\n"
"QCheckBox { font-size: 22px;}"));
        decrypt->setIconSize(QSize(16, 16));
        decrypt->setTristate(false);
        cryptOpts = new QFrame(encryptFrame);
        cryptOpts->setObjectName(QString::fromUtf8("cryptOpts"));
        cryptOpts->setGeometry(QRect(0, 140, 351, 341));
        cryptOpts->setStyleSheet(QString::fromUtf8(""));
        cryptOpts->setFrameShape(QFrame::StyledPanel);
        cryptOpts->setFrameShadow(QFrame::Raised);
        aesGen = new QPushButton(cryptOpts);
        aesGen->setObjectName(QString::fromUtf8("aesGen"));
        aesGen->setGeometry(QRect(220, 180, 91, 25));
        QFont font1;
        font1.setFamily(QString::fromUtf8("SansSerif"));
        font1.setPointSize(10);
        aesGen->setFont(font1);
        simpKey = new QLineEdit(cryptOpts);
        simpKey->setObjectName(QString::fromUtf8("simpKey"));
        simpKey->setGeometry(QRect(60, 70, 151, 25));
        simpKey->setFont(font1);
        simpGen = new QPushButton(cryptOpts);
        simpGen->setObjectName(QString::fromUtf8("simpGen"));
        simpGen->setGeometry(QRect(220, 70, 91, 25));
        simpGen->setFont(font1);
        simp = new QRadioButton(cryptOpts);
        simp->setObjectName(QString::fromUtf8("simp"));
        simp->setGeometry(QRect(40, 40, 112, 23));
        simp->setFont(font);
        simp->setStyleSheet(QString::fromUtf8("QRadioButton::indicator { width:15px; height: 15px;}\n"
"\n"
"QRadioButton { font-size: 20px; }"));
        aesKey = new QLineEdit(cryptOpts);
        aesKey->setObjectName(QString::fromUtf8("aesKey"));
        aesKey->setGeometry(QRect(60, 180, 151, 25));
        aesKey->setFont(font1);
        aes = new QRadioButton(cryptOpts);
        aes->setObjectName(QString::fromUtf8("aes"));
        aes->setGeometry(QRect(40, 150, 112, 23));
        aes->setFont(font);
        aes->setStyleSheet(QString::fromUtf8("QRadioButton::indicator { width:15px; height: 15px;}\n"
"\n"
"QRadioButton { font-size: 20px; }"));
        cryptOpts->raise();
        encrypt->raise();
        decrypt->raise();
        concealFrame = new QFrame(centralwidget);
        concealFrame->setObjectName(QString::fromUtf8("concealFrame"));
        concealFrame->setGeometry(QRect(670, 0, 251, 491));
        concealFrame->setStyleSheet(QString::fromUtf8("QFrame {border: none; }\n"
"QPushButton {background-color: #b5b5b5;color:  #383838;}\n"
"QLineEdit {background-color: #b5b5b5;color:  #383838;}"));
        concealFrame->setFrameShape(QFrame::StyledPanel);
        concealFrame->setFrameShadow(QFrame::Raised);
        conceal = new QCheckBox(concealFrame);
        conceal->setObjectName(QString::fromUtf8("conceal"));
        conceal->setGeometry(QRect(10, 30, 171, 64));
        conceal->setFont(font);
        conceal->setStyleSheet(QString::fromUtf8("QCheckBox::indicator { width:20px; height: 20px; }\n"
"\n"
"QCheckBox { font-size: 30px; font-style: bold;}"));
        conceal->setIconSize(QSize(16, 16));
        conceal->setTristate(false);
        reveal = new QCheckBox(concealFrame);
        reveal->setObjectName(QString::fromUtf8("reveal"));
        reveal->setGeometry(QRect(20, 110, 171, 21));
        reveal->setFont(font);
        reveal->setStyleSheet(QString::fromUtf8("QCheckBox::indicator { width:15px; height: 15px;}\n"
"\n"
"QCheckBox { font-size: 22px;}"));
        reveal->setIconSize(QSize(16, 16));
        concOpts = new QFrame(concealFrame);
        concOpts->setObjectName(QString::fromUtf8("concOpts"));
        concOpts->setGeometry(QRect(10, 130, 241, 351));
        concOpts->setFrameShape(QFrame::StyledPanel);
        concOpts->setFrameShadow(QFrame::Raised);
        slctImgFrame = new QFrame(concOpts);
        slctImgFrame->setObjectName(QString::fromUtf8("slctImgFrame"));
        slctImgFrame->setGeometry(QRect(0, 170, 241, 171));
        slctImgFrame->setFrameShape(QFrame::StyledPanel);
        slctImgFrame->setFrameShadow(QFrame::Raised);
        slctImgTxt = new QLineEdit(slctImgFrame);
        slctImgTxt->setObjectName(QString::fromUtf8("slctImgTxt"));
        slctImgTxt->setGeometry(QRect(20, 140, 131, 25));
        slctImgTxt->setFont(font1);
        genImg = new QRadioButton(slctImgFrame);
        genImg->setObjectName(QString::fromUtf8("genImg"));
        genImg->setGeometry(QRect(0, 80, 171, 21));
        genImg->setFont(font);
        genImg->setStyleSheet(QString::fromUtf8("QRadioButton::indicator { width:15px; height: 15px;}\n"
"\n"
"QRadioButton { font-size: 20px; }"));
        genImg->setIconSize(QSize(16, 16));
        slctImg = new QRadioButton(slctImgFrame);
        slctImg->setObjectName(QString::fromUtf8("slctImg"));
        slctImg->setGeometry(QRect(0, 110, 171, 21));
        slctImg->setFont(font);
        slctImg->setStyleSheet(QString::fromUtf8("QRadioButton::indicator { width:15px; height: 15px;}\n"
"\n"
"QRadioButton { font-size: 20px; }"));
        slctImg->setIconSize(QSize(16, 16));
        slctImgBrowse = new QPushButton(slctImgFrame);
        slctImgBrowse->setObjectName(QString::fromUtf8("slctImgBrowse"));
        slctImgBrowse->setGeometry(QRect(160, 140, 81, 25));
        slctImgBrowse->setFont(font1);
        slctImgBrowse->setStyleSheet(QString::fromUtf8(""));
        outImgName = new QLineEdit(slctImgFrame);
        outImgName->setObjectName(QString::fromUtf8("outImgName"));
        outImgName->setGeometry(QRect(0, 20, 131, 25));
        outImgName->setFont(font1);
        seededGen = new QPushButton(concOpts);
        seededGen->setObjectName(QString::fromUtf8("seededGen"));
        seededGen->setGeometry(QRect(160, 110, 81, 25));
        seededGen->setFont(font1);
        seededKey = new QLineEdit(concOpts);
        seededKey->setObjectName(QString::fromUtf8("seededKey"));
        seededKey->setGeometry(QRect(20, 110, 131, 25));
        seededKey->setFont(font1);
        frame_2 = new QFrame(concOpts);
        frame_2->setObjectName(QString::fromUtf8("frame_2"));
        frame_2->setGeometry(QRect(-10, 50, 241, 51));
        frame_2->setFrameShape(QFrame::StyledPanel);
        frame_2->setFrameShadow(QFrame::Raised);
        seeded = new QRadioButton(frame_2);
        seeded->setObjectName(QString::fromUtf8("seeded"));
        seeded->setGeometry(QRect(10, 30, 112, 23));
        seeded->setFont(font);
        seeded->setStyleSheet(QString::fromUtf8("QRadioButton::indicator { width:15px; height: 15px;}\n"
"\n"
"QRadioButton { font-size: 20px; }"));
        lsb = new QRadioButton(frame_2);
        lsb->setObjectName(QString::fromUtf8("lsb"));
        lsb->setGeometry(QRect(10, 0, 112, 23));
        lsb->setFont(font);
        lsb->setStyleSheet(QString::fromUtf8("QRadioButton::indicator { width:15px; height: 15px;}\n"
"\n"
"QRadioButton { font-size: 20px; }"));
        frame_2->raise();
        slctImgFrame->raise();
        seededGen->raise();
        seededKey->raise();
        concOpts->raise();
        conceal->raise();
        reveal->raise();
        universalFrame = new QFrame(centralwidget);
        universalFrame->setObjectName(QString::fromUtf8("universalFrame"));
        universalFrame->setGeometry(QRect(0, 390, 931, 191));
        universalFrame->setStyleSheet(QString::fromUtf8("QFrame {border: none;}"));
        universalFrame->setFrameShape(QFrame::StyledPanel);
        universalFrame->setFrameShadow(QFrame::Raised);
        start = new QPushButton(universalFrame);
        start->setObjectName(QString::fromUtf8("start"));
        start->setGeometry(QRect(10, 104, 911, 81));
        start->setFont(font);
        start->setStyleSheet(QString::fromUtf8("QPushButton {background-color: green; font-size: 40px; font-style: bold;}"));
        frame = new QFrame(centralwidget);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setGeometry(QRect(10, 20, 311, 371));
        frame->setStyleSheet(QString::fromUtf8("QFrame {border: none;}\n"
"QPushButton {background-color: #b5b5b5;color:  #383838;}\n"
"QLineEdit {background-color: #b5b5b5;color:  #383838;}"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        compress = new QCheckBox(frame);
        compress->setObjectName(QString::fromUtf8("compress"));
        compress->setGeometry(QRect(10, 10, 171, 64));
        compress->setFont(font);
        compress->setStyleSheet(QString::fromUtf8("QCheckBox::indicator { width:20px; height: 20px;}\n"
"\n"
"QCheckBox { font-size: 30px; font-style: bold;}"));
        compress->setIconSize(QSize(16, 16));
        compress->setTristate(false);
        decompress = new QCheckBox(frame);
        decompress->setObjectName(QString::fromUtf8("decompress"));
        decompress->setGeometry(QRect(20, 70, 171, 64));
        decompress->setFont(font);
        decompress->setStyleSheet(QString::fromUtf8("QCheckBox::indicator { width:15px; height: 15px;}\n"
"\n"
"QCheckBox { font-size: 22px;}"));
        decompress->setIconSize(QSize(16, 16));
        decompress->setTristate(false);
        outFileBrowse = new QPushButton(frame);
        outFileBrowse->setObjectName(QString::fromUtf8("outFileBrowse"));
        outFileBrowse->setGeometry(QRect(200, 180, 89, 25));
        outFileBrowse->setFont(font1);
        outFile = new QLineEdit(frame);
        outFile->setObjectName(QString::fromUtf8("outFile"));
        outFile->setGeometry(QRect(10, 180, 181, 25));
        outFile->setFont(font1);
        inFile = new QLineEdit(frame);
        inFile->setObjectName(QString::fromUtf8("inFile"));
        inFile->setGeometry(QRect(10, 140, 181, 25));
        inFile->setFont(font1);
        inFileBrowse = new QPushButton(frame);
        inFileBrowse->setObjectName(QString::fromUtf8("inFileBrowse"));
        inFileBrowse->setGeometry(QRect(200, 140, 89, 25));
        inFileBrowse->setFont(font1);
        MainWindow->setCentralWidget(centralwidget);
        universalFrame->raise();
        frame->raise();
        concealFrame->raise();
        encryptFrame->raise();
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 927, 26));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "cZip", nullptr));
        encrypt->setText(QCoreApplication::translate("MainWindow", "Encrypt", nullptr));
        decrypt->setText(QCoreApplication::translate("MainWindow", "Decrypt", nullptr));
        aesGen->setText(QCoreApplication::translate("MainWindow", "Generate", nullptr));
        simpKey->setText(QString());
        simpKey->setPlaceholderText(QCoreApplication::translate("MainWindow", "key", nullptr));
        simpGen->setText(QCoreApplication::translate("MainWindow", "Generate", nullptr));
        simp->setText(QCoreApplication::translate("MainWindow", "Simple", nullptr));
        aesKey->setText(QString());
        aesKey->setPlaceholderText(QCoreApplication::translate("MainWindow", "key", nullptr));
        aes->setText(QCoreApplication::translate("MainWindow", "AES", nullptr));
        conceal->setText(QCoreApplication::translate("MainWindow", "Conceal", nullptr));
        reveal->setText(QCoreApplication::translate("MainWindow", "Reveal", nullptr));
        slctImgTxt->setText(QString());
        slctImgTxt->setPlaceholderText(QCoreApplication::translate("MainWindow", "image file", nullptr));
        genImg->setText(QCoreApplication::translate("MainWindow", "Generate Image", nullptr));
        slctImg->setText(QCoreApplication::translate("MainWindow", "Select Image", nullptr));
        slctImgBrowse->setText(QCoreApplication::translate("MainWindow", "Browse", nullptr));
        outImgName->setText(QString());
        outImgName->setPlaceholderText(QCoreApplication::translate("MainWindow", "output name", nullptr));
        seededGen->setText(QCoreApplication::translate("MainWindow", "Generate", nullptr));
        seededKey->setText(QString());
        seededKey->setPlaceholderText(QCoreApplication::translate("MainWindow", "key", nullptr));
        seeded->setText(QCoreApplication::translate("MainWindow", "Seeded", nullptr));
        lsb->setText(QCoreApplication::translate("MainWindow", "LSB", nullptr));
        start->setText(QCoreApplication::translate("MainWindow", "Start", nullptr));
        compress->setText(QCoreApplication::translate("MainWindow", "Compress", nullptr));
        decompress->setText(QCoreApplication::translate("MainWindow", "Decompress", nullptr));
        outFileBrowse->setText(QCoreApplication::translate("MainWindow", "Browse", nullptr));
        outFile->setText(QString());
        outFile->setPlaceholderText(QCoreApplication::translate("MainWindow", "output folder", nullptr));
        inFile->setText(QString());
        inFile->setPlaceholderText(QCoreApplication::translate("MainWindow", "input file", nullptr));
        inFileBrowse->setText(QCoreApplication::translate("MainWindow", "Browse", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
