#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QtWidgets>
#include <QThread>
#include <QtCrypto>
#include "simplecrypt.h"

Q_DECLARE_METATYPE(QCA::KeyStoreEntry);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->cryptOpts->setHidden(true);
    ui->concOpts->setHidden(true);    
}

MainWindow::~MainWindow()
{
    delete ui;
}


// file select slots

void MainWindow::on_inFileBrowse_clicked()
{
    QString file_name = QFileDialog::getOpenFileName(this, "Open a file", QDir::homePath());
    ui->inFile->setText(file_name);
}


void MainWindow::on_outFileBrowse_clicked()
{
    QString file_name = QFileDialog::getExistingDirectory(this, "Open destination", QDir::homePath(), QFileDialog::ShowDirsOnly);
    ui->outFile->setText(file_name + QString("/"));
}


void MainWindow::on_slctImgBrowse_clicked()
{
    QString file_name = QFileDialog::getOpenFileName(this, "Open a file", QDir::homePath(), tr("Image Files (*.png)"));
    ui->slctImgTxt->setText(file_name);
}

// compression functionality
void MainWindow::compress(std::string& base_name) {
    QFile inFS(ui->inFile->text());
    if(!inFS.open(QFile::ReadOnly)) {
        QMessageBox messageBox;
        messageBox.setText("Error, can't open source file");
        messageBox.exec();
    }
    int compress_level;
    const qint64 GB =  1073741824;
    // compression speed tiered by file size
    if(inFS.size() < 15 * GB)
        compress_level = 9;
    else if(inFS.size() < 50 * GB)
        compress_level= 6;
    else
        compress_level = 3;
    // obtain base name (no directory tree or extension)
    std::string inFileText = ui->inFile->text().toStdString();
    base_name = inFileText.substr(inFileText.find_last_of("/\\"));
    std::string::size_type const c(base_name.find_last_of('.'));
    if(c != 0)
        base_name = base_name.substr(0, c);
    std::string outFileName = ui->outFile->text().toStdString();
    QFile oFS(QString((outFileName + base_name + ".cz").c_str()));
    if(!oFS.open(QFile::WriteOnly)) {
        QMessageBox messageBox;
        messageBox.setText("Error, can't create output file");
        messageBox.exec();
    }
    // write compressed file
    oFS.write(qCompress(inFS.readAll(), compress_level));
    inFS.close();
    oFS.close();
    return;
}

// SimpleCrypt functionality
void MainWindow::encrypt_simple(std::string& base_name) {
    const quint64 num = QString(QString("0x" + ui->simpKey->text())).toInt();  // convert hex seed to quint64
    SimpleCrypt crypto;
    // set up crypto engine
    crypto.setKey(num);
    crypto.setCompressionMode(SimpleCrypt::CompressionNever);  // file pre compressed
    crypto.setIntegrityProtectionMode(SimpleCrypt::ProtectionHash);
    // prepare output file
    QFile inCrypt(ui->outFile->text() + QString((base_name + ".cz").c_str()));
    if(!inCrypt.open(QFile::ReadOnly)) {
        QMessageBox messageBox;
        messageBox.setText("Error, can't open source file");
        messageBox.exec();
    }

    QFile outCrypt(ui->outFile->text() + QString((base_name + ".pcrypt").c_str()));
    if(!outCrypt.open(QFile::WriteOnly)) {
        QMessageBox messageBox;
        messageBox.setText("Error, can't open dest file");
        messageBox.exec();
    }
    // convert encrypted sequence to bytearray and output
    QByteArray e = crypto.encryptToByteArray(inCrypt.readAll());

    outCrypt.write(e);
    inCrypt.close();
    outCrypt.close();
    return;
}

// AES functionality
void MainWindow::encrypt_aes(std::string& base_name) {
    // key and initialization vector are sent together
    std::string k = ui->aesKey->text().toStdString().substr(0,64);
    std::string v = ui->aesKey->text().toStdString().substr(64, 32);

    QString kb(k.c_str());
    QString vb(v.c_str());
    // convert key string to bytearray equivalent of hex values
    QCA::SymmetricKey key(QByteArray::fromHex(kb.toLatin1()));
    QCA::InitializationVector iv(QByteArray::fromHex(vb.toLatin1()));
    // set up IO files
    QFile inCrypt(ui->outFile->text() + QString((base_name + ".cz").c_str()));
    if(!inCrypt.open(QFile::ReadOnly)) {
        QMessageBox messageBox;
        messageBox.setText("Error, can't open source file");
        messageBox.exec();
    }

    QFile outCrypt(ui->outFile->text() + QString((base_name + ".acrypt").c_str()));
    if(!outCrypt.open(QFile::WriteOnly)) {
        QMessageBox messageBox;
        messageBox.setText("Error, can't open dest file");
        messageBox.exec();
    }

    const QByteArray fileRead = inCrypt.readAll();
    inCrypt.close();
    // read file 14 bytes at a time, encrypt with 2 bytes of padding, write to output file
    // for all contents of fileRead
    QByteArray seg;
    seg.resize(14);
    for(int i = 0; i < fileRead.size(); i = i + 14) {
        for(int j = 0; j < 14; ++j) {
            if(i + j < fileRead.size())
                seg[j] = fileRead[i + j];
            else
                seg[j] = 0x00;
        }
        QCA::SecureArray arg(seg);


        QCA::Cipher cipher(QStringLiteral("aes256"), QCA::Cipher::CBC, QCA::Cipher::DefaultPadding, QCA::Encode, key, iv);
        QCA::SecureArray u = cipher.update(arg);
        if(!cipher.ok()) {
            QMessageBox messageBox;
            messageBox.setText("Error during encrpytion");
            messageBox.exec();
        }
        QCA::SecureArray f = cipher.final();
        if(!cipher.ok()) {
            QMessageBox messageBox;QCA::Initializer init = QCA::Initializer();
            messageBox.setText("Error during finalization");
            messageBox.exec();
        }
    QByteArray out = f.toByteArray();
    outCrypt.write(out);
    }
    outCrypt.close();
    return;
}

// image generation algorithm
void MainWindow::gen_img(QImage& img, int size) {
    srand(time(NULL));
    while(img.isNull() || img.width() * img.height() < size * 2 + 1 || img.width() * img.height() > size * 8)
        img = QImage(rand() % 32000, rand() % 32000, QImage::Format_ARGB32);
    for(int y = 0; y < img.height(); ++y) {
       QRgb *line = (QRgb *) img.scanLine(y);
       for (int x = 0; x < img.width(); ++x) {
          line[x] = QColor(rand() % 253, rand() % 255, rand() % 255, 255).rgb();
       }
    }
    return;
}

// LSB steganography functionality
void MainWindow::stego_lsb(std::string& base_name) {
    bool gen = false;
    QImage img;
    // load user-selected image if desired
    if(ui->slctImg->isChecked()) {
        img = QImage (ui->slctImgTxt->text());
        if(img.isNull()) {
           QMessageBox messageBox;
           messageBox.setText("Error, can't open source image");
           messageBox.exec();
        }
        img = img.convertToFormat(QImage::Format_ARGB32);
     }
    else
        gen = true;

    QString hideFile;
    // decide which input field to use based on previous methods enacted
    if(!(ui->compress->isChecked()) && !(ui->encrypt->isChecked()))
        hideFile = ui->inFile->text();
    else if(!(ui->encrypt->isChecked()))
        hideFile = ui->outFile->text() + QString(base_name.c_str()) + QString(".cz");
    else if(ui->simp->isChecked())
        hideFile = ui->outFile->text() + QString(base_name.c_str()) + QString(".pcrypt");
    else
        hideFile = ui->outFile->text() + QString(base_name.c_str()) + QString(".acrypt");

    QFile inHide(hideFile);
    inHide.open(QFile::ReadOnly);
    QByteArray bytes(inHide.readAll());
    inHide.close();

    QVector<bool> b;  // holds bits of in file
    char mask;
    // populate bit vector
    for(int i = 0; i < bytes.size(); ++i) {
        for(int j = 0; j < 8; ++j) {
            mask = (0x01 << j);
            b.push_back((bytes[i] & mask) == mask);
        }
    }
    // if generated image desired, populate image with AT LEAST as many pixels as bits in in file and only as many as 1.5x
    if(gen) {
       gen_img(img, bytes.size());
    }
    // check if image can fit file, used for user selected image
    if(img.height() * img.width() < (b.size() / 4) + 1) {
        QMessageBox messageBox;
        messageBox.setText("Error, image too small to hold file");
        messageBox.exec();
    }
    // ensure bits are divisible by 4 (should always be for most standard files)
    // if it isn't, pad end with 0s
    while(b.size() % 4 != 0) {
        b.push_back(0);
    }
    // for each bit in bit vector, overwrite least significant bit of each color value with bit info (4 per pixel)
    QRgb *pixel = (QRgb *)img.bits();
    int z = 0;
    for(int i = 0; i < b.size() / 4; ++i) {
       if(qRed(pixel[i]) >= 254)
           pixel[i] = qRgba(qRed(pixel[i]) - 2, qGreen(pixel[i]), qBlue(pixel[i]), qAlpha(pixel[i]));
       int color[4] = {qRed(pixel[i]), qGreen(pixel[i]), qBlue(pixel[i]), qAlpha(pixel[i])};
       for(int j = 0; j < 4; ++j) {
           color[j] &= 0xFE;
           color[j] |= (b[z] == 1) ? 0x01 : 0x00;
           ++z;
       }
       pixel[i] = qRgba(color[0], color[1], color[2], color[3]);  // load updated pixel back to image
    }
    pixel[b.size() / 4] = QColor(255,255,255).rgb();  // pixel after final bit is updated to fully opaque black pixel
    // save output image
    if(!(img.save(ui->outFile->text() + ui->outImgName->text() + QString(".png")))) {
        QMessageBox messageBox;
        messageBox.setText("Error, can't save ouput image");
        messageBox.exec();
    }
    return;
}

// seeded steganography functionality
void MainWindow::stego_seed(std::string& base_name) {
    // used to easily compare characters from seed input field
    std::map<char, QVector<int>> hex = {{'0', {0, 1, 2, 3}}, {'1', {0, 1, 3, 2}}, {'2', {0, 3, 1, 2}}, {'3', {0, 2, 3, 1}},
                                  {'4', {1, 0, 2, 3}}, {'5',{1, 0, 3, 2}}, {'6', {1, 3, 0, 2}}, {'7', {1, 2, 0, 3}},
                                  {'8', {2, 0, 1, 3}}, {'9', {2, 0, 3, 1}}, {'a', {2, 3, 0, 1}}, {'b', {2, 3, 1, 0}},
                                  {'c', {3, 0, 1, 2}}, {'d', {3, 0, 2, 1}}, {'e', {3, 2, 0, 1}}, {'f', {3, 2, 1, 0}}};

    bool gen = false;
    QImage img;
    // load user selected image if desired
    if(ui->slctImg->isChecked()) {
        img = QImage(ui->slctImgTxt->text());
        if(img.isNull()) {
           QMessageBox messageBox;
           messageBox.setText("Error, can't open source image");
           messageBox.exec();
        }
        img = img.convertToFormat(QImage::Format_ARGB32);
     }
    else
        gen = true;

    QString hideFile;
    // decide which input field to use based on previous methods enacted
    if(!(ui->compress->isChecked()) && !(ui->encrypt->isChecked()))
        hideFile = ui->inFile->text();
    else if(!(ui->encrypt->isChecked()))
        hideFile = ui->outFile->text() + QString(base_name.c_str()) + QString(".cz");
    else if(ui->simp->isChecked())
        hideFile = ui->outFile->text() + QString(base_name.c_str()) + QString(".pcrypt");
    else
        hideFile = ui->outFile->text() + QString(base_name.c_str()) + QString(".acrypt");

    QFile inHide(hideFile);
    inHide.open(QFile::ReadOnly);
    QByteArray bytes(inHide.readAll());
    inHide.close();

    QString seed(ui->seededKey->text());
    int x = 0;
    QVector<bool> b;
    char mask;
    // populate bit vector and sequence vector
    for(int i = 0; i < bytes.size(); ++i) {
        for(int j = 0; j < 8; ++j) {
            mask = (0x01 << j);
            b.push_back((bytes[i] & mask) == mask);
        }
    }
    // generate random image
    if(gen) {
        gen_img(img, bytes.size());
    }

    if(img.height() * img.width() < (b.size() / 4) + 1) {
        QMessageBox messageBox;
        messageBox.setText("Error, image too small to hide file");
        messageBox.exec();
    }
    // ensure bits are divisible by 4 (should always be for most standard files)
    // if it isn't, pad end with 0s
    while(b.size() % 4 != 0) {
        b.push_back(0);
    }
    // for seeded algorithm, the user specified seed is used to determine what order the pixels will have
    // their least significant bits overwritten. Once the end of the seed is reached,
    // the sequence pattern is restarted
    int z = 0;
    QRgb *pixel = (QRgb *)img.bits();
    for(int i = 0; i < b.size() / 4; ++i) {
        if(qRed(pixel[i]) >= 254)
            pixel[i] = qRgba(qRed(pixel[i]) - 2, qGreen(pixel[i]), qBlue(pixel[i]), qAlpha(pixel[i]));
        int color[4] = {qRed(pixel[i]), qGreen(pixel[i]), qBlue(pixel[i]), qAlpha(pixel[i])};
        for(int j = 0; j < 4; ++j) {
            color[j] &= 0xFE;
            color[j] |= (b[z + hex[QString(seed[x]).toStdString()[0]][j]] == 1) ? 0x01 : 0x00;
        }
        z += 4;
        ++x;
        if(x > seed.size() - 1)
            x = 0;
        pixel[i] = qRgba(color[0], color[1], color[2], color[3]);
    }
    pixel[b.size() / 4] = QColor(255,255,255).rgb();  // black pixel for pixel after last bit
    // output image
    if(!(img.save(ui->outFile->text() + ui->outImgName->text() + QString(".png")))) {
        QMessageBox messageBox;
        messageBox.setText("Error, can't save output image");
        messageBox.exec();
    }
    return;
}

// LSB reveal functionality
void MainWindow::rev_lsb(std::string& base_name) {
    // load image
    QImage img(ui->inFile->text());
    if(img.isNull()) {
        QMessageBox messageBox;
        messageBox.setText("Error, can't open source image");
        messageBox.exec();
    }
    img = img.convertToFormat(QImage::Format_ARGB32);
    qint8 mask = 0x01;
    QBitArray bvec;  // holds extracted bits
    bvec.resize(img.width() * img.height() * 4);  // file can be as large as number of pixels * 4
    bvec.fill(0);
    // loop through image pixels and extract least significant bit value of
    // each pixel color value until a black pixel is reached
    QRgb *pixel = (QRgb *)img.bits();
    int i = 0;
    int z = 0;
    while(pixel[i] != QColor(255,255,255).rgb()) {
        int color[4] = {qRed(pixel[i]), qGreen(pixel[i]), qBlue(pixel[i]), qAlpha(pixel[i])};
        for(int j = 0; j < 4; ++j) {
            bvec[z] = ((color[j] & mask) == mask);
            ++z;
        }
        ++i;
    }
    bvec.resize(z);  // resize bit vector to actual size
    QByteArray bytes;
    bytes.resize(bvec.count()/8+1);
    bytes.fill(0);
    // Convert from QBitArray to QByteArray
    for(int b=0; b<bvec.count(); ++b)
      bytes[b/8] = ( bytes.at(b/8) | ((bvec[b]?1:0)<<(b%8)));
    // trim excess null data
    if(bytes[bytes.size() -1] == QByteArray(1, 0)[0])
        bytes.resize(bytes.size() - 1);
    // generate output name
    std::string inFileText = ui->inFile->text().toStdString();
    base_name = inFileText.substr(inFileText.find_last_of("/\\"));
    std::string::size_type const c(base_name.find_last_of('.'));
    if(c != 0)
        base_name = base_name.substr(0, c);
    std::string outFileName = ui->outFile->text().toStdString();
    // output to file
    QFile revFile(QString((outFileName + base_name + ".r").c_str()));
    if(!revFile.open(QFile::WriteOnly)) {
        QMessageBox messageBox;
        messageBox.setText("Error, can't create output file");
        messageBox.exec();
    }
    revFile.write(bytes);
    revFile.close();
    return;
}

// seeded reveal functionality
void MainWindow::rev_seed(std::string& base_name) {
    // map hard coded as before
    std::map<char, QVector<int>> hex = {{'0', {0, 1, 2, 3}}, {'1', {0, 1, 3, 2}}, {'2', {0, 3, 1, 2}}, {'3', {0, 2, 3, 1}},
                                  {'4', {1, 0, 2, 3}}, {'5',{1, 0, 3, 2}}, {'6', {1, 3, 0, 2}}, {'7', {1, 2, 0, 3}},
                                  {'8', {2, 0, 1, 3}}, {'9', {2, 0, 3, 1}}, {'a', {2, 3, 0, 1}}, {'b', {2, 3, 1, 0}},
                                  {'c', {3, 0, 1, 2}}, {'d', {3, 0, 2, 1}}, {'e', {3, 2, 0, 1}}, {'f', {3, 2, 1, 0}}};
    // load image
    QImage img(ui->inFile->text());
    if(img.isNull()) {
        QMessageBox messageBox;
        messageBox.setText("Error, can't open source image");
        messageBox.exec();
    }
    img = img.convertToFormat(QImage::Format_ARGB32);
    qint8 mask = 0x01;
    QBitArray bvec;  // extracted bit vector
    bvec.resize(img.width() * img.height() * 4);
    bvec.fill(0);
    // loop through image pixels and extract least significant bit value of
    // each seed-specified-color pixel pattern until a black pixel is reached
    QRgb *pixel = (QRgb *)img.bits();
    QString seed(ui->seededKey->text());
    int i = 0;
    int x = 0;
    int z = 0;
    while(pixel[i] != QColor(255,255,255,255).rgb()) {
        int color[4] = {qRed(pixel[i]), qGreen(pixel[i]), qBlue(pixel[i]), qAlpha(pixel[i])};
        for(int j = 0; j < 4; ++j) {
            bvec[z + hex[QString(seed[x]).toStdString()[0]][j]] = ((color[j] & mask) == mask);
        }
        z += 4;
        ++i;
        ++x;
        if(x > seed.size() - 1)
            x = 0;
    }
    bvec.resize(z);
    QByteArray bytes;
    bytes.resize(bvec.count()/8+1);
    bytes.fill(0);
    // Convert from QBitArray to QByteArray
    for(int b=0; b<bvec.count(); ++b)
      bytes[b/8] = ( bytes.at(b/8) | ((bvec[b]?1:0)<<(b%8)));
    // trim excess data
    if(bytes[bytes.size() -1] == QByteArray(1, 0)[0])
        bytes.resize(bytes.size() - 1);
    // generate output name
    std::string inFileText = ui->inFile->text().toStdString();
    base_name = inFileText.substr(inFileText.find_last_of("/\\"));
    std::string::size_type const c(base_name.find_last_of('.'));
    if(c != 0)
        base_name = base_name.substr(0, c);
    std::string outFileName = ui->outFile->text().toStdString();
    QFile revFile(QString((outFileName + base_name + ".r").c_str()));
    // output to file
    if(!revFile.open(QFile::WriteOnly)) {
        QMessageBox messageBox;
        messageBox.setText("Error, can't create output file");
        messageBox.exec();
    }
    revFile.write(bytes);
    revFile.close();
    return;
}

// SimpleCrypt decryption functionality
void MainWindow::decrypt_simple(std::string base_name) {
    QFile inCrypt;
    QFile outCrypt;
    qint64 num = QString(QString("0x" + ui->simpKey->text())).toUInt();  // translate key to qint64 from hex values
    SimpleCrypt crypto(num);
    // determine input name from previous methods
    if(ui->reveal->isChecked())
        inCrypt.setFileName(ui->outFile->text() + QString(base_name.c_str()) + QString(".r"));
    else
        inCrypt.setFileName(ui->inFile->text());
    if(!inCrypt.open(QFile::ReadOnly)) {
        QMessageBox messageBox;
        messageBox.setText("Error, can't open source file");
        messageBox.exec();
    }
    // determine output name from previous methods
    if(!ui->reveal->isChecked()) {
        std::string inFileText = ui->inFile->text().toStdString();
        base_name = inFileText.substr(inFileText.find_last_of("/\\"));
        std::string::size_type const c(base_name.find_last_of('.'));
        if(c != 0)
            base_name = base_name.substr(0, c);
        std::string outFileName = ui->outFile->text().toStdString();
        outCrypt.setFileName(ui->outFile->text() + QString(base_name.c_str()) + QString(".dcrypt"));
    }
    else
        outCrypt.setFileName(ui->outFile->text() + QString(base_name.c_str()) + QString(".dcrypt"));

    if(!outCrypt.open(QFile::WriteOnly)) {
        QMessageBox messageBox;
        messageBox.setText("Error, can't open dest file");
        messageBox.exec();
    }

    // convert decrypted file to byte array and output to file
    QByteArray e = crypto.decryptToByteArray(inCrypt.readAll());

    outCrypt.write(e);
    inCrypt.close();
    outCrypt.close();
    return;
}

// AES decryption functionality
void MainWindow::decrypt_aes(std::string& base_name) {
    QFile inCrypt;
    QFile outCrypt;

    std::string k = ui->aesKey->text().toStdString().substr(0,64);
    std::string v = ui->aesKey->text().toStdString().substr(64, 32);

    QString kb(k.c_str());
    QString vb(v.c_str());
    // load key  and initialization vector from hex string provided
    QCA::SymmetricKey key(QByteArray::fromHex(kb.toLatin1()));
    QCA::InitializationVector iv(QByteArray::fromHex(vb.toLatin1()));
    // setup input file
    if(ui->reveal->isChecked())
        inCrypt.setFileName(ui->outFile->text() + QString(base_name.c_str()) + QString(".r"));
    else
        inCrypt.setFileName(ui->inFile->text());
    if(!inCrypt.open(QFile::ReadOnly)) {
        QMessageBox messageBox;
        messageBox.setText("Error, can't open source file");
        messageBox.exec();
    }
    // retrieve base name if needed and setup output file
    if(!ui->reveal->isChecked()) {
        std::string inFileText = ui->inFile->text().toStdString();
        base_name = inFileText.substr(inFileText.find_last_of("/\\"));
        std::string::size_type const c(base_name.find_last_of('.'));
        if(c != 0)
            base_name = base_name.substr(0, c);
        std::string outFileName = ui->outFile->text().toStdString();
        outCrypt.setFileName(ui->outFile->text() + QString((base_name + ".dcrypt").c_str()));
    }
    else
        outCrypt.setFileName(ui->outFile->text() + QString(base_name.c_str()) + QString(".dcrypt"));
    if(!outCrypt.open(QFile::WriteOnly)) {
        QMessageBox messageBox;
        messageBox.setText("Error, can't open dest file");
        messageBox.exec();
    }
    const QByteArray fileRead = inCrypt.readAll();
    inCrypt.close();
    // read file in 16 bytes at a time and output each decrypted block to file
    QByteArray seg;
    seg.resize(16);
    for(int i = 0; i < fileRead.size(); i = i + 16) {
        for(int j = 0; j < 16; ++j) {
            if(i + j < fileRead.size())
                seg[j] = fileRead[i + j];
            else
                seg[j] = 0x00;
            }
            QCA::SecureArray arg(seg);

            QCA::Cipher cipher(QStringLiteral("aes256"), QCA::Cipher::CBC, QCA::Cipher::DefaultPadding, QCA::Encode, key, iv);
            cipher.setup(QCA::Decode, key, iv);
            QCA::SecureArray u = cipher.update(arg);
            if(!cipher.ok()) {
                QMessageBox messageBox;
                messageBox.setText("Error during encrpytion");
                messageBox.exec();
            }
            QCA::SecureArray f = cipher.final();
            if(!cipher.ok()) {
                QMessageBox messageBox;QCA::Initializer init = QCA::Initializer();
                messageBox.setText("Error during finalization");
                messageBox.exec();
                return;
            }

    QByteArray out = f.toByteArray();
    outCrypt.write(out);
    }
    outCrypt.close();
    return;
}

// decompression functionality
void MainWindow::decompress(std::string& base_name) {
    QFile inFS;
    // use previous methods to determine input field
    if(ui->reveal->isChecked() && !ui->decrypt->isChecked())
        inFS.setFileName(ui->outFile->text() + QString(base_name.c_str()) + QString(".r"));
    else if(ui->reveal->isChecked() && ui->decrypt->isChecked())
        inFS.setFileName(ui->outFile->text() + QString(base_name.c_str()) + QString(".dcrypt"));
    else
        inFS.setFileName(ui->inFile->text());


    if(!inFS.open(QFile::ReadOnly)) {
       QMessageBox messageBox;
       messageBox.setText("Error, can't open source file");
       messageBox.exec();
    }
    // use previous methods to determine output name
    if(!ui->reveal->isChecked() && !ui->decrypt->isChecked()) {
       std::string inFileText = ui->inFile->text().toStdString();
       base_name = inFileText.substr(inFileText.find_last_of("/\\"));
       std::string::size_type const c(base_name.find_last_of('.'));
       if(c != 0)
          base_name = base_name.substr(0, c);
    }
    std::string outFileName = ui->outFile->text().toStdString();
    QFile oFS(QString((outFileName + base_name + ".uzip").c_str()));
    if(!oFS.open(QFile::WriteOnly)) {
       QMessageBox messageBox;
       messageBox.setText("Error, can't create output file");
       messageBox.exec();
    }
    // write decompressed file
    oFS.write(qUncompress(inFS.readAll()));
    inFS.close();
    oFS.close();
    return;
}

// main program functionality
void MainWindow::on_start_clicked()
{
    std::string base_name;
    // compression sequence
    if(ui->compress->isChecked() && !(ui->decrypt->isChecked())) {
        compress(base_name);
    }
    // encryption sequence
    if(ui->encrypt->isChecked() && !(ui->decrypt->isChecked())) {
        QCA::Initializer init = QCA::Initializer();
        // SimpleCrypt implementation
        if(ui->simp->isChecked()) {
            encrypt_simple(base_name);
        }
        // AES256 implementation
        else if(ui->aes->isChecked()) {
            encrypt_aes(base_name);
        }
    }
    // steganography algorithms
    if(ui->conceal->isChecked() && !(ui->reveal->isChecked())) {
        // LSB implementation
        if(ui->lsb->isChecked()) {
            stego_lsb(base_name);
        }
        // seeded implementation
        else {
            stego_seed(base_name);
        }
    }
    // reverse steganography methods
    if(ui->reveal->isChecked()) {
        // LSB implementation
        if(ui->lsb->isChecked()) {
            rev_lsb(base_name);
        }
        // seeded implementation
        else {
            rev_seed(base_name);
        }
    }
    // decrpytion methods
    if(ui->decrypt->isChecked()) {
        QCA::Initializer init = QCA::Initializer();
        // SimpleCrypt implementation
        if(ui->simp->isChecked()) {
            decrypt_simple(base_name);
        }
        // AES 256 implementation
        else if(ui->aes->isChecked()) {
            decrypt_aes(base_name);
        }
        }
    // decompression method
    if(ui->decompress->isChecked()) {
        decompress(base_name);
    }
    QMessageBox messageBox;
    messageBox.setText("Operation Successful!");
    messageBox.exec();
}

// key generation slots

void MainWindow::on_aesGen_clicked()
{
    QCA::Initializer init = QCA::Initializer();
    QCA::SymmetricKey key(32);
    QCA::InitializationVector iv(16);
    QString k(key.toByteArray().toHex().data());
    QString v(iv.toByteArray().toHex().data());
    QString kv = k + v;
    ui->aesKey->setText(kv);
    return;
}


void MainWindow::on_seededGen_clicked()
{
    QCA::Initializer init = QCA::Initializer();
    QCA::SymmetricKey key(32);
    QString k(key.toByteArray().toHex().data());
    ui->seededKey->setText(k);
    return;
}



void MainWindow::on_simpGen_clicked()
{
    QCA::Initializer init = QCA::Initializer();
    QCA::SymmetricKey key(8);
    QString k(key.toByteArray().toHex().data());
    ui->simpKey->setText(k);
    return;
}

// following slots ensure only correct sequence of buttons can be selected at once
// hides unnecessary entry fields

void MainWindow::on_compress_clicked()
{
    if(ui->compress->isChecked()) {
        if(ui->decompress->isChecked())
            ui->decompress->setCheckState(Qt::Unchecked);
        if(ui->decrypt->isChecked()) {
            ui->decrypt->setCheckState(Qt::Unchecked);
            if(!ui->encrypt->isChecked())
                ui->cryptOpts->setHidden(true);
        }
        if(ui->reveal->isChecked()) {
            ui->reveal->setCheckState(Qt::Unchecked);
            if(!ui->conceal->isChecked())
                ui->concOpts->setHidden(true);
        }
    }
    if(ui->encrypt->isChecked())
        ui->compress->setCheckState(Qt::Checked);
}


void MainWindow::on_decompress_clicked()
{
    if(ui->decompress->isChecked()) {
        if(ui->compress->isChecked())
            ui->compress->setCheckState(Qt::Unchecked);
        if(ui->conceal->isChecked()) {
            ui->conceal->setCheckState(Qt::Unchecked);
            if(!ui->reveal->isChecked())
                ui->concOpts->setHidden(true);
        }
        if(ui->encrypt->isChecked()) {
            ui->encrypt->setCheckState(Qt::Unchecked);
            if(!ui->decrypt->isChecked())
                ui->cryptOpts->setHidden(true);
        }
    }
    if(ui->decrypt->isChecked())
        ui->decompress->setCheckState(Qt::Checked);
}


void MainWindow::on_encrypt_clicked()
{
    if(ui->encrypt->isChecked()) {
        ui->cryptOpts->setHidden(false);
        if(ui->decrypt->isChecked())
            ui->decrypt->setCheckState(Qt::Unchecked);
        if(ui->decompress->isChecked())
            ui->decompress->setCheckState(Qt::Unchecked);
        if(ui->reveal->isChecked()) {
            ui->reveal->setCheckState(Qt::Unchecked);
            if(!ui->conceal->isChecked())
                ui->concOpts->setHidden(true);
        }
        if(!ui->compress->isChecked())
            ui->compress->setCheckState(Qt::Checked);
    }
    else {
        if(!ui->decrypt->isChecked())
            ui->cryptOpts->setHidden(true);
    }
}


void MainWindow::on_decrypt_clicked()
{
    if(ui->decrypt->isChecked()) {
        ui->cryptOpts->setHidden(false);
        if(ui->encrypt->isChecked()) {
            ui->encrypt->setCheckState(Qt::Unchecked);
            if(!ui->decrypt->isChecked())
                ui->cryptOpts->setHidden(true);
        }
        if(ui->compress->isChecked())
            ui->compress->setCheckState(Qt::Unchecked);
        if(ui->conceal->isChecked()) {
            ui->conceal->setCheckState(Qt::Unchecked);
            if(!ui->reveal->isChecked())
                ui->concOpts->setHidden(true);
        }
        if(!ui->decompress->isChecked())
            ui->decompress->setCheckState(Qt::Checked);
    }
    else {
        if(!ui->encrypt->isChecked())
            ui->cryptOpts->setHidden(true);
    }
}


void MainWindow::on_conceal_clicked()
{
    if(ui->conceal->isChecked()) {
        ui->concOpts->setHidden(false);
        ui->slctImgFrame->setHidden(false);
        if(ui->reveal->isChecked())
            ui->reveal->setCheckState(Qt::Unchecked);
        if(ui->decrypt->isChecked()) {
            ui->decrypt->setCheckState(Qt::Unchecked);
            if(!ui->encrypt->isChecked())
                ui->cryptOpts->setHidden(true);
        }
        if(ui->decompress->isChecked())
            ui->decompress->setCheckState(Qt::Unchecked);
    }
    else {
        if(!ui->reveal->isChecked())
            ui->concOpts->setHidden(true);
    }
}


void MainWindow::on_reveal_clicked()
{
    if(ui->reveal->isChecked()) {
        ui->slctImgFrame->setHidden(true);
        ui->concOpts->setHidden(false);
        if(ui->conceal->isChecked())
            ui->conceal->setCheckState(Qt::Unchecked);
        if(ui->compress->isChecked())
            ui->compress->setCheckState(Qt::Unchecked);
        if(ui->encrypt->isChecked()) {
            ui->encrypt->setCheckState(Qt::Unchecked);
            if(!ui->decrypt->isChecked())
                ui->cryptOpts->setHidden(true);
        }
    }
    else {
        if(!ui->conceal->isChecked())
            ui->concOpts->setHidden(true);
        else
            ui->slctImgFrame->setHidden(false);

    }
}

