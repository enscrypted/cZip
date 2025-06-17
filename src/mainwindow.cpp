#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QtWidgets>
#include <QThread>
#include <QFileInfo>
#include <QDataStream>
#include <QMessageBox>
#include <random>
#include <cmath>
#include <cstring>

#ifdef Q_OS_MACX
  #include <qca.h>
#else
  #include <QtCrypto>
#endif

#include "simplecrypt.h"

Q_DECLARE_METATYPE(QCA::KeyStoreEntry);

//
// constructor / destructor
//

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

//
// main program functionality
//

// orchestrates the selected processing chain
void MainWindow::on_start_clicked()
{
    ProcessingJob job;
    if(!initialize_job(job)) {
        return;
    }

    bool success = true;

    // determine workflow based on the primary user action
    // workflow 1: reveal is the highest priority starting action
    if (ui->reveal->isChecked()) {
        // start by revealing the data from the source image
        if (ui->aura->isChecked()) {
            success = aura_reveal(job);
        } else if (ui->lsb->isChecked()) {
            success = rev_lsb(job);
        } else {
            display_error("Input Error", "Please select a reveal algorithm (AURA or LSB).");
            success = false;
        }

        if (success && (ui->decrypt->isChecked() || ui->decompress->isChecked())) {
            QList<CZipFormat::OperationCode> op_stack = read_operation_stack(job.currentFile);

            if(op_stack.isEmpty()){
                display_error("File Error", "The file extracted from the image is not a valid cZip container or has no further operations to reverse.");
                success = false;
            } else {
                // process the stack of the newly revealed file
                for (const auto& op : op_stack) {
                    if (!success) break;
                    // only perform the operations the user actually checked
                    if(ui->decrypt->isChecked() && op == CZipFormat::OperationCode::ENCRYPT_AES) { success = decrypt_aes(job); }
                    if(ui->decrypt->isChecked() && op == CZipFormat::OperationCode::ENCRYPT_SIMPLE) { success = decrypt_simple(job); }
                    if(ui->decompress->isChecked() && op == CZipFormat::OperationCode::COMPRESS_ZLIB) { success = decompress(job); }
                }
            }
        }
    }
    // workflow 2: forward chain (compress / encrypt / conceal)
    else if (ui->compress->isChecked() || ui->encrypt->isChecked() || ui->conceal->isChecked()) {
        if (ui->compress->isChecked()) {
            if (success) success = compress(job);
        }
        if (ui->encrypt->isChecked()) {
            if (success) {
                if (ui->aes->isChecked()) success = encrypt_aes(job);
                else success = encrypt_simple(job);
            }
        }
        if (ui->conceal->isChecked()) {
            if (success) {
                if (ui->aura->isChecked()) success = aura_conceal(job);
                else success = stego_lsb(job);
            }
        }
    }
    // workflow 3: standalone reverse (decrypt / decompress on a .czip file)
    else if (ui->decrypt->isChecked() || ui->decompress->isChecked()) {
         QList<CZipFormat::OperationCode> op_stack = read_operation_stack(job.originalFile);
         if(op_stack.isEmpty()){
             display_error("File Error", "The selected file is not a valid cZip file.");
             success = false;
         } else {
             for (const auto& op : op_stack) {
                 if (!success) break;
                 if(ui->decrypt->isChecked() && op == CZipFormat::OperationCode::ENCRYPT_AES) success = decrypt_aes(job);
                 if(ui->decrypt->isChecked() && op == CZipFormat::OperationCode::ENCRYPT_SIMPLE) success = decrypt_simple(job);
                 if(ui->decompress->isChecked() && op == CZipFormat::OperationCode::COMPRESS_ZLIB) success = decompress(job);
             }
         }
    }
    else {
        display_error("No Action", "Please select an operation to perform.");
        return;
    }

    // finalize and cleanup
    if (success) {
            QList<QString> files_to_delete;
            for (const QString& file_path : job.interimFiles) {
                if (file_path != job.currentFile) { // do not delete the final result
                    files_to_delete.append(file_path);
                }
            }
            for (const QString& file_path : files_to_delete) {
                QFile file(file_path);
                if(file.exists()) {
                    file.remove();
                }
            }
            QMessageBox::information(this, "Success", "Operation completed successfully.");
        } else {
            // always clean up all interim files on failure
            for (const QString& file_path : job.interimFiles) {
                QFile file(file_path);
                if(file.exists()) {
                    file.remove();
                }
            }
        }
}

//
// job management and helper functions
//

// populates the job struct from the ui and validates input
bool MainWindow::initialize_job(ProcessingJob& job)
{
    job.originalFile = ui->inFile->text();
    job.currentFile = ui->inFile->text();
    job.outputFolder = ui->outFile->text();

    if(job.originalFile.isEmpty()) {
        display_error("Input Error", "Please select an input file.");
        return false;
    }

    if(job.outputFolder.isEmpty()) {
        display_error("Input Error", "Please select an output folder.");
        return false;
    }

    if(!job.outputFolder.endsWith('/') && !job.outputFolder.endsWith('\\')) {
        job.outputFolder += '/';
    }

    return true;
}

// cleans up all intermediate files after a successful run
void MainWindow::cleanup_interim_files(ProcessingJob& job)
{
    for (const QString& file_path : job.interimFiles) {
        QFile file(file_path);
        if(file.exists()) {
            file.remove();
        }
    }
}

QList<CZipFormat::OperationCode> MainWindow::read_operation_stack(const QString& filePath)
{
    QList<CZipFormat::OperationCode> operations;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        display_error("File Error", "Could not open file to read operation history.");
        return operations;
    }

    QDataStream stream(&file);
    stream.setByteOrder(QDataStream::BigEndian);

    CZipFormat::FileHeader header;

    // confirm we can read the fixed-size part of the header
    if(stream.readRawData(reinterpret_cast<char*>(&header), sizeof(header)) != sizeof(header)) {
        return operations;
    }

    // verify the magic number to confirm it's our file
    if(memcmp(header.magic, CZipFormat::MAGIC, sizeof(header.magic)) != 0) {
        return operations;
    }

    for(uint8_t i = 0; i < header.op_count; ++i) {
        uint8_t op_code_byte;
        stream >> op_code_byte;
        if(stream.status() != QDataStream::Ok) {
            display_error("File Error", "Failed to read the full operation stack from the file header.");
            return QList<CZipFormat::OperationCode>();
        }
        operations.append(static_cast<CZipFormat::OperationCode>(op_code_byte));
    }

    return operations;
}

// checks if a file exceeds the 2gib limit of qt5
bool MainWindow::check_file_size(const QString& path)
{
    const qint64 two_gibibytes = 2147483648;
    if (QFileInfo(path).size() > two_gibibytes) {
        display_error("File Too Large", "Files larger than 2GiB are not supported in this version.");
        return false;
    }
    return true;
}

// reads and parses a .czip container file into a struct
bool MainWindow::read_czip_container(ProcessingJob& job, CzipContainer& container)
{
    QFile input_file(job.currentFile);
    if (!input_file.open(QIODevice::ReadOnly)) {
        display_error("File Error", "Could not open source file: " + job.currentFile);
        return false;
    }

    QDataStream in_stream(&input_file);
    in_stream.setByteOrder(QDataStream::BigEndian);

    CZipFormat::FileHeader header;
    if (input_file.peek(sizeof(header.magic)) != QByteArray(CZipFormat::MAGIC, sizeof(CZipFormat::MAGIC))) {
        // This is not a czip file, treat the whole thing as a payload
        container.payload = input_file.readAll();
        container.original_filename = QFileInfo(job.originalFile).fileName();
        return true;
    }

    // It is a czip file, so parse it fully
    in_stream.readRawData(reinterpret_cast<char*>(&header), sizeof(header));
    if (header.version != CZipFormat::VERSION) {
        display_error("Version Error", "Unsupported cZip file format version.");
        return false;
    }

    for (uint8_t i = 0; i < header.op_count; ++i) {
        uint8_t op_code_byte;
        in_stream >> op_code_byte;
        container.op_stack.append(static_cast<CZipFormat::OperationCode>(op_code_byte));
    }

    uint16_t filename_len;
    in_stream >> filename_len;
    QByteArray filename_utf8;
    filename_utf8.resize(filename_len);
    in_stream.readRawData(filename_utf8.data(), filename_len);
    container.original_filename = QString::fromUtf8(filename_utf8);
    container.payload = input_file.readAll();

    return true;
}

// writes a new .czip container file with an updated header and new payload
bool MainWindow::write_czip_container(ProcessingJob& job, const QByteArray& payload, QList<CZipFormat::OperationCode>& op_stack)
{
    QString original_base_name = QFileInfo(job.originalFile).baseName();
    QString new_file_path = job.outputFolder + original_base_name + ".czip";

    QFile output_file(new_file_path);
    if (!output_file.open(QIODevice::WriteOnly)) {
        display_error("File Error", "Could not create output file: " + new_file_path);
        return false;
    }

    QDataStream out_stream(&output_file);
    out_stream.setByteOrder(QDataStream::BigEndian);

    // write header
    out_stream.writeRawData(CZipFormat::MAGIC, sizeof(CZipFormat::MAGIC));
    out_stream << CZipFormat::VERSION;
    out_stream << (uint8_t)op_stack.size();
    for (const auto op_code : op_stack) {
        out_stream << (uint8_t)op_code;
    }
    QByteArray original_filename_utf8 = QFileInfo(job.originalFile).fileName().toUtf8();
    out_stream << (uint16_t)original_filename_utf8.size();
    out_stream.writeRawData(original_filename_utf8.constData(), original_filename_utf8.size());

    // write payload
    output_file.write(payload);
    output_file.close();

    // update job state
    if(job.currentFile != job.originalFile) {
        job.interimFiles.append(job.currentFile);
    }
    job.currentFile = new_file_path;

    return true;
}

// displays an error message to the user
void MainWindow::display_error(const QString& title, const QString& message)
{
    QMessageBox::critical(this, title, message);
}

// extracts raw lsb data from image, including the size header
QByteArray MainWindow::extract_raw_lsb_data(const QImage& image)
{
    QByteArray revealed_data_buffer;
    unsigned char current_byte_assembly = 0;
    int bits_in_current_byte = 0; // count of bits assembled for current_byte_assembly (0-7)
    quint64 embedded_czip_size = 0;
    bool size_header_extracted = false;
    const int SIZE_HEADER_BYTE_LENGTH = sizeof(quint64);

    QImage processed_image = image.convertToFormat(QImage::Format_ARGB32);
    QRgb *pixels = (QRgb*)processed_image.bits();
    qint64 total_pixels = processed_image.width() * processed_image.height();

    // iterate through pixels to extract embedded bits
    for (qint64 i = 0; i < total_pixels; ++i) {
        // if we've already extracted the size and enough data, stop
        if (size_header_extracted && revealed_data_buffer.size() >= (qint64)embedded_czip_size) {
             revealed_data_buffer.resize(embedded_czip_size);
             return revealed_data_buffer;
        }

        QRgb pixel = pixels[i];
        int r = qRed(pixel);
        int g = qGreen(pixel);
        int b = qBlue(pixel);

        // extract 3 bits per pixel (1 from r, 1 from g, 1 from b lsb)
        int extracted_bits[3];
        extracted_bits[0] = (r & 1); // lsb of red
        extracted_bits[1] = (g & 1); // lsb of green
        extracted_bits[2] = (b & 1); // lsb of blue

        for (int j = 0; j < 3; ++j) {
            // this check prevents processing bits beyond the known embedded data size
            if (size_header_extracted && revealed_data_buffer.size() >= (qint64)embedded_czip_size) {
                break;
            }

            unsigned char extracted_bit_val = static_cast<unsigned char>(extracted_bits[j]); // the bit extracted from the pixel lsb

            // assemble byte by placing extracted bit at the correct MSB position
            current_byte_assembly |= (extracted_bit_val << (7 - bits_in_current_byte));
            bits_in_current_byte++;

            if (bits_in_current_byte == 8) {
                // once a full byte is assembled, append it to the buffer
                revealed_data_buffer.append(static_cast<char>(current_byte_assembly)); // cast back to char for qbytearray
                current_byte_assembly = 0; // reset for next byte
                bits_in_current_byte = 0;

                // after extracting enough bytes for the size header, parse it
                if (!size_header_extracted && revealed_data_buffer.size() >= SIZE_HEADER_BYTE_LENGTH) {
                    QDataStream size_stream(&revealed_data_buffer, QIODevice::ReadOnly);
                    size_stream.setByteOrder(QDataStream::BigEndian);
                    size_stream >> embedded_czip_size; // read the expected size of the czip container
                    size_header_extracted = true;

                    // trim the size header bytes from the buffer
                    revealed_data_buffer = revealed_data_buffer.mid(SIZE_HEADER_BYTE_LENGTH);

                    // check for invalid extracted size
                    if (embedded_czip_size == 0 ||
                        (qint64) embedded_czip_size > (total_pixels * 3 / 8) - SIZE_HEADER_BYTE_LENGTH) {
                        display_error("LSB Reveal Error", "Invalid embedded data size detected. Image may be corrupt or not a valid LSB container.");
                        revealed_data_buffer.clear(); // clear buffer on error
                        return revealed_data_buffer;
                    }
                }
            }
        }
    }

    // final check if all expected data was extracted before loop finished
    if (size_header_extracted && revealed_data_buffer.size() < (qint64)embedded_czip_size) {
        display_error("LSB Reveal Error", "Incomplete data extracted from image. Image may be truncated or corrupt.");
        revealed_data_buffer.clear(); // clear buffer on error
        return revealed_data_buffer;
    }

    // if size header was never extracted, or no data was extracted at all
    if (!size_header_extracted && !revealed_data_buffer.isEmpty()) {
         display_error("LSB Reveal Error", "Could not extract full data size header from image.");
         revealed_data_buffer.clear();
         return revealed_data_buffer;
    } else if (revealed_data_buffer.isEmpty() && total_pixels > 0) {
        display_error("LSB Reveal Error", "No data extracted from image.");
        return revealed_data_buffer;
    }

    return revealed_data_buffer;
}

// prepares the full payload (czip header + data + size header) for lsb embedding
QByteArray MainWindow::prepare_lsb_payload(CzipContainer& container)
{
    // prepend lsb concealment to the operation stack
    container.op_stack.prepend(CZipFormat::OperationCode::CONCEAL_LSB);

    // serialize the entire container (header + payload) into a single byte array for embedding
    QByteArray data_to_embed;
    QDataStream out_stream(&data_to_embed, QIODevice::WriteOnly);
    out_stream.setByteOrder(QDataStream::BigEndian);

    // write the standard czip header
    out_stream.writeRawData(CZipFormat::MAGIC, sizeof(CZipFormat::MAGIC));
    out_stream << CZipFormat::VERSION;
    out_stream << (uint8_t)container.op_stack.size();
    for (const auto op_code : container.op_stack) {
        out_stream << (uint8_t)op_code;
    }
    QByteArray original_filename_utf8 = container.original_filename.toUtf8();
    out_stream << (uint16_t)original_filename_utf8.size();
    out_stream.writeRawData(original_filename_utf8.constData(), original_filename_utf8.size());
    out_stream.writeRawData(container.payload.constData(), container.payload.size());


    // prepend the size of the embedded data to data_to_embed
    QByteArray size_header_bytes;
    QDataStream size_stream(&size_header_bytes, QIODevice::WriteOnly);
    size_stream.setByteOrder(QDataStream::BigEndian);
    size_stream << (quint64)data_to_embed.size(); // Store the size of the full cZip container

    data_to_embed.prepend(size_header_bytes);

    return data_to_embed;
}

// embeds the raw data (including size header and czip container) into the image pixels
bool MainWindow::embed_raw_lsb_data(QImage& image, const QByteArray& data_to_embed)
{
    qint64 data_size_bits = data_to_embed.size() * 8;

    qint64 current_global_bit_index = 0;
    QRgb *pixels = (QRgb*)image.bits(); // direct pixel access
    qint64 total_pixels = image.width() * image.height();

    // iterate through pixels and embed data bit by bit
    for (qint64 i = 0; i < total_pixels; ++i) {
        if (current_global_bit_index >= data_size_bits) {
            break; // all data embedded
        }

        QRgb pixel = pixels[i];
        int r = qRed(pixel);
        int g = qGreen(pixel);
        int b = qBlue(pixel);
        int a = qAlpha(pixel);

        // red
        // get the current source byte for this bit
        quint8 current_source_byte_r = static_cast<quint8>(data_to_embed.at(current_global_bit_index / 8));
        // calculate bit offset within the source byte (msb is 7, lsb is 0)
        int bit_offset_in_source_byte_r = 7 - (current_global_bit_index % 8);
        int bit_r = (current_source_byte_r >> bit_offset_in_source_byte_r) & 1;
        r = (r & 0xFE) | bit_r;
        current_global_bit_index++;


        // green
        if (current_global_bit_index < data_size_bits) {
            quint8 current_source_byte_g = static_cast<quint8>(data_to_embed.at(current_global_bit_index / 8));
            int bit_offset_in_source_byte_g = 7 - (current_global_bit_index % 8);
            int bit_g = (current_source_byte_g >> bit_offset_in_source_byte_g) & 1;
            g = (g & 0xFE) | bit_g;
            current_global_bit_index++;
        }

        // blue
        if (current_global_bit_index < data_size_bits) {
            quint8 current_source_byte_b = static_cast<quint8>(data_to_embed.at(current_global_bit_index / 8));
            int bit_offset_in_source_byte_b = 7 - (current_global_bit_index % 8);
            int bit_b = (current_source_byte_b >> bit_offset_in_source_byte_b) & 1;
            b = (b & 0xFE) | bit_b;
            current_global_bit_index++;
        }

        pixels[i] = qRgba(r, g, b, a); // update pixel
    }

    if (current_global_bit_index < data_size_bits) {
        display_error("LSB Steganography Error", "Failed to embed all data. Image was unexpectedly too small.");
        return false;
    }
    return true;
}

// processes a qbytearray that is expected to be a czip container
bool MainWindow::process_extracted_czip_data(ProcessingJob& job, QByteArray& extractedData)
{
    QDataStream czip_data_stream(extractedData);
    czip_data_stream.setByteOrder(QDataStream::BigEndian);

    CZipFormat::FileHeader header;
    // confirm magic number to ensure it's a valid czip container
    if (czip_data_stream.readRawData(reinterpret_cast<char*>(&header), sizeof(header)) != sizeof(header) ||
        memcmp(header.magic, CZipFormat::MAGIC, sizeof(header.magic)) != 0) {
        display_error("LSB Reveal Error", "Extracted data is not a valid cZip container.");
        return false;
    }

    // read the operation stack from the revealed czip header
    QList<CZipFormat::OperationCode> op_stack;
    for(uint8_t i = 0; i < header.op_count; ++i) {
        uint8_t op_code_byte;
        czip_data_stream >> op_code_byte;
        op_stack.append(static_cast<CZipFormat::OperationCode>(op_code_byte));
    }

    // read the original filename length and actual filename
    uint16_t filename_len;
    czip_data_stream >> filename_len;
    QByteArray original_filename_utf8;
    original_filename_utf8.resize(filename_len);
    czip_data_stream.readRawData(original_filename_utf8.data(), filename_len);
    QString original_filename = QString::fromUtf8(original_filename_utf8);

    // the remaining data is the payload
    QByteArray payload = extractedData.mid(czip_data_stream.device()->pos());

    // determine the final output path
    QString final_output_path = job.outputFolder + original_filename;

    // if the stack contains other ops, it's an intermediate .czip.
    bool lsb_only_in_stack = (op_stack.size() == 1 && op_stack.first() == CZipFormat::OperationCode::CONCEAL_LSB);

    if (lsb_only_in_stack) {
        QFile output_file(final_output_path);
        if (!output_file.open(QIODevice::WriteOnly)) {
            display_error("LSB Reveal Error", "Could not create output file: " + final_output_path);
            return false;
        }
        // write only the payload when it's the final output for LSB-only reveal
        output_file.write(payload);
        output_file.close();
        job.currentFile = final_output_path; // update currentfile to the final output
    } else {
        if (!op_stack.isEmpty() && op_stack.first() == CZipFormat::OperationCode::CONCEAL_LSB) {
            op_stack.removeFirst();
        } else {
            // This case indicates a logic error or corrupted stack, LSB reveal should always be first if it's there
            display_error("LSB Reveal Error", "Unexpected operation stack order after LSB reveal.");
            return false;
        }

        // now write the new intermediate .czip file with the updated op_stack
        QString revealed_czip_path = job.outputFolder + QFileInfo(job.originalFile).baseName() + ".revealed.czip";
        QFile output_file(revealed_czip_path);
        if (!output_file.open(QIODevice::WriteOnly)) {
            display_error("LSB Reveal Error", "Could not create temporary revealed .czip file: " + revealed_czip_path);
            return false;
        }

        QDataStream out_stream(&output_file);
        out_stream.setByteOrder(QDataStream::BigEndian);

        // write new header with updated op_stack
        out_stream.writeRawData(CZipFormat::MAGIC, sizeof(CZipFormat::MAGIC));
        out_stream << CZipFormat::VERSION;
        out_stream << (uint8_t)op_stack.size();
        for (const auto op_code : op_stack) {
            out_stream << (uint8_t)op_code;
        }
        out_stream << (uint16_t)original_filename_utf8.size();
        out_stream.writeRawData(original_filename_utf8.constData(), original_filename_utf8.size());

        output_file.write(payload);
        output_file.close();
        job.currentFile = revealed_czip_path;
    }

    if(job.currentFile != job.originalFile) {
        job.interimFiles.append(job.currentFile);
    }

    return true;
}

//
// forward processing functions (compress, encrypt, conceal)
//

// handles compression and czip container creation
bool MainWindow::compress(ProcessingJob& job)
{
    if(!check_file_size(job.currentFile)) { return false; }

    CzipContainer container;
    if(!read_czip_container(job, container)) { return false; }

    QByteArray compressed_payload = qCompress(container.payload, 9);

    container.op_stack.prepend(CZipFormat::OperationCode::COMPRESS_ZLIB);

    return write_czip_container(job, compressed_payload, container.op_stack);
}

// handles simplecrypt encryption
bool MainWindow::encrypt_simple(ProcessingJob& job)
{
    if(!check_file_size(job.currentFile)) { return false; }

    CzipContainer container;
    if(!read_czip_container(job, container)) { return false; }

    SimpleCrypt crypto(ui->simpKey->text().toULongLong());
    QByteArray encrypted_payload = crypto.encryptToByteArray(container.payload);

    container.op_stack.prepend(CZipFormat::OperationCode::ENCRYPT_SIMPLE);

    return write_czip_container(job, encrypted_payload, container.op_stack);
}

// handles aes encryption
bool MainWindow::encrypt_aes(ProcessingJob& job)
{
    if(!check_file_size(job.currentFile)) { return false; }

    CzipContainer container;
    if(!read_czip_container(job, container)) { return false; }

    QByteArray passphrase = ui->aesKey->text().toUtf8();
    QByteArray key_data = QCA::Hash("sha256").hash(passphrase).toByteArray();
    QByteArray key = key_data.left(32);
    QByteArray iv = key_data.mid(0, 16);

    QCA::Cipher cipher(QStringLiteral("aes256"), QCA::Cipher::CBC,
                       QCA::Cipher::PKCS7, QCA::Encode, key, iv);

    QByteArray encrypted_payload = cipher.update(container.payload).toByteArray();
    encrypted_payload += cipher.final().toByteArray();

    if(!cipher.ok()) {
        display_error("AES Encryption Error", "QCA failed to encrypt the data.");
        return false;
    }

    container.op_stack.prepend(CZipFormat::OperationCode::ENCRYPT_AES);

    return write_czip_container(job, encrypted_payload, container.op_stack);
}

// handles lsb steganography
bool MainWindow::stego_lsb(ProcessingJob& job)
{
    // read current file into czipcontainer for standardized processing
    CzipContainer container;
    if(!read_czip_container(job, container)) {
        return false;
    }

    // prepare the full payload (czip header + data + size header) for embedding
    QByteArray data_to_embed = prepare_lsb_payload(container);
    if (data_to_embed.isEmpty()) {
        // error message handled within prepare_lsb_payload if necessary
        return false;
    }

    // initial file size check before image operations
    if(!check_file_size(job.currentFile)) { return false; }

    qint64 data_size_bits = data_to_embed.size() * 8;

    QImage image;
    if(ui->genImg->isChecked()) {
        // calculate required pixels: 1 pixel (3 color channels r,g,b) embeds 3 bits
        qint64 required_pixels = (data_size_bits / 3) + 1;
        image = gen_img(required_pixels);
    } else {
        image.load(ui->slctImgTxt->text());
    }

    if(image.isNull()) {
        display_error("Image Error", "Could not load or generate the cover image.");
        return false;
    }

    image = image.convertToFormat(QImage::Format_ARGB32);

    // check if the image has sufficient capacity for the data
    if ((qint64)(image.width() * image.height() * 3) < data_size_bits) {
        display_error("Image Error", "The selected image is too small to hold the data.");
        return false;
    }

    // embed the prepared data into the image
    if (!embed_raw_lsb_data(image, data_to_embed)) {
        // error message handled within embed_raw_lsb_data
        return false;
    }

    // save the final steganographic image
    QString final_path = job.outputFolder + ui->outImgName->text() + ".png";
    if(!image.save(final_path)) {
        display_error("Save Error", "Could not save the final image file.");
        return false;
    }

    if(job.currentFile != job.originalFile) {
        job.interimFiles.append(job.currentFile);
    }
    job.currentFile = final_path;

    return true;
}

// handles aura concealment
bool MainWindow::aura_conceal(ProcessingJob& job)
{
    std::vector<unsigned char> master_key = hex_to_vector(ui->auraKey->text());
    if(master_key.size() != AURA_Processor::KEY_SIZE) {
        aura_error(AURA_Result::Error_Invalid_Input);
        return false;
    }

    QFile input_file(job.currentFile);
    if(!input_file.open(QIODevice::ReadOnly)) {
        display_error("AURA Error", "Could not open source file for concealment.");
        return false;
    }

    // prepare payload for aura library, including a simple header
    QByteArray file_content = input_file.readAll();
    QString original_filename = QFileInfo(input_file).fileName();
    QByteArray payload_data;
    QDataStream stream(&payload_data, QIODevice::WriteOnly);
    stream << (quint64)0;
    stream << (quint16)original_filename.toUtf8().size();
    payload_data.append(original_filename.toUtf8());
    payload_data.append(file_content);
    stream.device()->seek(0);
    stream << (quint64)payload_data.size();
    std::vector<unsigned char> payload_vec(payload_data.begin(), payload_data.end());
    if(job.currentFile != job.originalFile) {
        job.interimFiles.append(job.currentFile);
    }

    // prepare cover image
    QImage image;
    if(ui->genImg->isChecked()) {
        image = gen_img(AURA_Processor::calculate_required_pixels(payload_vec.size()));
    } else {
        image.load(ui->slctImgTxt->text());
    }

    if(image.isNull()) {
        display_error("Image Error", "Could not load or generate the cover image.");
        return false;
    }
    if((size_t)(image.width() * image.height()) < AURA_Processor::calculate_required_pixels(payload_vec.size())) {
        aura_error(AURA_Result::Error_Image_Too_Small);
        return false;
    }
    image = image.convertToFormat(QImage::Format_ARGB32);

    // call the aura library to perform encryption and embedding
    AURA_ImageBuffer image_buffer { image.bits(), (size_t)image.width(), (size_t)image.height() };
    AURA_Processor aura(master_key);
    AURA_Result result = aura.encrypt(payload_vec, image_buffer);

    if (result != AURA_Result::Success) {
        aura_error(result);
        return false;
    }

    QString final_path = job.outputFolder + ui->outImgName->text() + ".png";
    if(!image.save(final_path)) {
        display_error("Save Error", "Could not save the final image file.");
        return false;
    }

    job.currentFile = final_path;
    return true;
}

//
// reverse processing functions (decompress, decrypt, reveal)
//

// decompression logic
bool MainWindow::decompress(ProcessingJob& job)
{
    QFile input_file(job.currentFile);
    if (!input_file.open(QIODevice::ReadOnly)) {
        display_error("Decompression Error", "Could not open source file: " + job.currentFile);
        return false;
    }

    QDataStream in_stream(&input_file);
    in_stream.setByteOrder(QDataStream::BigEndian);

    // read and validate the header
    CZipFormat::FileHeader header;
    if(in_stream.readRawData(reinterpret_cast<char*>(&header), sizeof(header)) != sizeof(header) ||
       memcmp(header.magic, CZipFormat::MAGIC, sizeof(header.magic)) != 0) {
        display_error("File Error", "The selected file is not a valid cZip file or is corrupted.");
        input_file.close();
        return false;
    }

    QList<CZipFormat::OperationCode> op_stack;
    for(uint8_t i = 0; i < header.op_count; ++i) {
        uint8_t op_code_byte;
        in_stream >> op_code_byte;
        op_stack.append(static_cast<CZipFormat::OperationCode>(op_code_byte));
    }

    if(op_stack.isEmpty() || op_stack.first() != CZipFormat::OperationCode::COMPRESS_ZLIB) {
        display_error("Operation Error", "File does not appear to be compressed. Cannot decompress.");
        input_file.close();
        return false;
    }

    uint16_t filename_len;
    in_stream >> filename_len;
    QByteArray original_filename_utf8;
    original_filename_utf8.resize(filename_len);
    in_stream.readRawData(original_filename_utf8.data(), filename_len);
    QString original_filename = QString::fromUtf8(original_filename_utf8);

    QByteArray compressed_payload = input_file.readAll();
    input_file.close();

    QByteArray decompressed_payload = qUncompress(compressed_payload);
    if(decompressed_payload.isEmpty() && !compressed_payload.isEmpty()) {
        display_error("Decompression Error", "qUncompress failed. The data may be corrupt.");
        return false;
    }

    if(job.currentFile != job.originalFile) {
        job.interimFiles.append(job.currentFile);
    }

    op_stack.removeFirst();

    if(op_stack.isEmpty()) {
        // restore the file with its original name
        QString final_path = job.outputFolder + original_filename;
        QFile output_file(final_path);
        if (!output_file.open(QIODevice::WriteOnly)) {
            display_error("Decompression Error", "Could not create final output file: " + final_path);
            return false;
        }
        output_file.write(decompressed_payload);
        output_file.close();
        job.currentFile = final_path;
    } else {
        // create a new intermediate .czip file
        QString base_name = QFileInfo(job.originalFile).baseName();
        QString new_file_path = job.outputFolder + base_name + ".czip";
        QFile output_file(new_file_path);
        if (!output_file.open(QIODevice::WriteOnly)) {
            display_error("Decompression Error", "Could not create intermediate output file: " + new_file_path);
            return false;
        }

        // write a new, updated header to the intermediate file
        QDataStream out_stream(&output_file);
        out_stream.setByteOrder(QDataStream::BigEndian);
        out_stream.writeRawData(CZipFormat::MAGIC, sizeof(CZipFormat::MAGIC));
        out_stream << CZipFormat::VERSION;
        out_stream << (uint8_t)op_stack.size();
        for(const auto op_code : op_stack) {
            out_stream << (uint8_t)op_code;
        }
        out_stream << (uint16_t)original_filename_utf8.size();
        out_stream.writeRawData(original_filename_utf8.constData(), original_filename_utf8.size());

        output_file.write(decompressed_payload);
        output_file.close();
        job.currentFile = new_file_path;
    }

    return true;
}

// handles simplecrypt decryption
bool MainWindow::decrypt_simple(ProcessingJob& job)
{
    QFile input_file(job.currentFile);
    if (!input_file.open(QIODevice::ReadOnly)) {
        display_error("Simple Decryption Error", "Could not open source file: " + job.currentFile);
        return false;
    }

    QDataStream in_stream(&input_file);
    in_stream.setByteOrder(QDataStream::BigEndian);

    // read and validate the file header
    CZipFormat::FileHeader header;
    if(in_stream.readRawData(reinterpret_cast<char*>(&header), sizeof(header)) != sizeof(header) ||
       memcmp(header.magic, CZipFormat::MAGIC, sizeof(header.magic)) != 0) {
        display_error("File Error", "The selected file is not a valid cZip file.");
        input_file.close();
        return false;
    }

    QList<CZipFormat::OperationCode> op_stack;
    for(uint8_t i = 0; i < header.op_count; ++i) {
        uint8_t op_code_byte;
        in_stream >> op_code_byte;
        op_stack.append(static_cast<CZipFormat::OperationCode>(op_code_byte));
    }

    if(op_stack.isEmpty() || op_stack.first() != CZipFormat::OperationCode::ENCRYPT_SIMPLE) {
        display_error("Operation Error", "File was not encrypted with the 'simple' algorithm.");
        input_file.close();
        return false;
    }

    uint16_t filename_len;
    in_stream >> filename_len;
    QByteArray original_filename_utf8;
    original_filename_utf8.resize(filename_len);
    in_stream.readRawData(original_filename_utf8.data(), filename_len);

    QByteArray encrypted_payload = input_file.readAll();
    SimpleCrypt crypto(ui->simpKey->text().toULongLong());
    QByteArray decrypted_payload = crypto.decryptToByteArray(encrypted_payload);
    input_file.close();

    // write new intermediate file with updated header
    QString original_base_name = QFileInfo(job.originalFile).baseName();
    QString new_file_path = job.outputFolder + original_base_name + ".czip";
    QFile output_file(new_file_path);
    if (!output_file.open(QIODevice::WriteOnly)) {
        display_error("Simple Decryption Error", "Could not create output file: " + new_file_path);
        return false;
    }
    QDataStream out_stream(&output_file);
    out_stream.setByteOrder(QDataStream::BigEndian);
    out_stream.writeRawData(CZipFormat::MAGIC, sizeof(CZipFormat::MAGIC));
    out_stream << CZipFormat::VERSION;

    op_stack.removeFirst();
    out_stream << (uint8_t)op_stack.size();
    for(const auto op_code : op_stack) {
        out_stream << (uint8_t)op_code;
    }

    out_stream << (uint16_t)original_filename_utf8.size();
    out_stream.writeRawData(original_filename_utf8.constData(), original_filename_utf8.size());
    output_file.write(decrypted_payload);
    output_file.close();

    if(job.currentFile != job.originalFile) {
        job.interimFiles.append(job.currentFile);
    }
    job.currentFile = new_file_path;

    return true;
}

// handles aes decryption
bool MainWindow::decrypt_aes(ProcessingJob& job) {
    QFile input_file(job.currentFile);
    if (!input_file.open(QIODevice::ReadOnly)) {
        display_error("AES Decryption Error", "Could not open source file: " + job.currentFile);
        return false;
    }

    QDataStream in_stream(&input_file);
    in_stream.setByteOrder(QDataStream::BigEndian);

    CZipFormat::FileHeader header;
    if(in_stream.readRawData(reinterpret_cast<char*>(&header), sizeof(header)) != sizeof(header) ||
       memcmp(header.magic, CZipFormat::MAGIC, sizeof(header.magic)) != 0) {
        display_error("File Error", "The selected file is not a valid cZip file or is corrupted.");
        input_file.close();
        return false;
    }

    QList<CZipFormat::OperationCode> op_stack;
    for(uint8_t i = 0; i < header.op_count; ++i) {
        uint8_t op_code_byte;
        in_stream >> op_code_byte;
        op_stack.append(static_cast<CZipFormat::OperationCode>(op_code_byte));
    }

    if(op_stack.isEmpty() || op_stack.first() != CZipFormat::OperationCode::ENCRYPT_AES) {
        display_error("Operation Error", "File was not encrypted with the AES algorithm. Cannot perform AES decryption.");
        input_file.close();
        return false;
    }

    uint16_t filename_len;
    in_stream >> filename_len;
    QByteArray original_filename_utf8;
    original_filename_utf8.resize(filename_len);
    in_stream.readRawData(original_filename_utf8.data(), filename_len);

    QByteArray encrypted_payload = input_file.readAll();
    input_file.close();

    // derive key and iv exactly as in encryption
    QByteArray passphrase = ui->aesKey->text().toUtf8();
    QByteArray key_data = QCA::Hash("sha256").hash(passphrase).toByteArray();
    QByteArray key = key_data.left(32);
    QByteArray iv = key_data.mid(0, 16);
    QCA::Cipher cipher(QStringLiteral("aes256"), QCA::Cipher::CBC,
                       QCA::Cipher::PKCS7, QCA::Decode, key, iv);

    QByteArray decrypted_payload = cipher.update(encrypted_payload).toByteArray();
    decrypted_payload += cipher.final().toByteArray();

    if(!cipher.ok()) {
        display_error("AES Decryption Error", "Decryption failed. The key is likely incorrect or the data is corrupt.");
        return false;
    }

    QString original_base_name = QFileInfo(job.originalFile).baseName();
    QString new_file_path = job.outputFolder + original_base_name + ".czip";
    QFile output_file(new_file_path);
    if (!output_file.open(QIODevice::WriteOnly)) {
        display_error("AES Decryption Error", "Could not create output file: " + new_file_path);
        return false;
    }
    QDataStream out_stream(&output_file);
    out_stream.setByteOrder(QDataStream::BigEndian);
    out_stream.writeRawData(CZipFormat::MAGIC, sizeof(CZipFormat::MAGIC));
    out_stream << CZipFormat::VERSION;
    op_stack.removeFirst();
    out_stream << (uint8_t)op_stack.size();
    for(const auto op_code : op_stack) {
        out_stream << (uint8_t)op_code;
    }
    out_stream << (uint16_t)original_filename_utf8.size();
    out_stream.writeRawData(original_filename_utf8.constData(), original_filename_utf8.size());
    output_file.write(decrypted_payload);
    output_file.close();

    if(job.currentFile != job.originalFile) {
        job.interimFiles.append(job.currentFile);
    }
    job.currentFile = new_file_path;

    return true;
}

// handles lsb reveal
bool MainWindow::rev_lsb(ProcessingJob& job)
{
    QImage image(job.currentFile);
    if(image.isNull()) {
        display_error("LSB Reveal Error", "Could not load the source image: " + job.currentFile);
        return false;
    }

    // extract raw lsb data from the image
    QByteArray extracted_data = extract_raw_lsb_data(image);
    if (extracted_data.isEmpty()) {
        display_error("LSB Reveal Error", "Failed to extract any data from the image.");
        return false;
    }

    // process the extracted czip container data
    // this handles saving as original file or intermediate .czip
    return process_extracted_czip_data(job, extracted_data);
}

// handles aura reveal
bool MainWindow::aura_reveal(ProcessingJob& job)
{
    std::vector<unsigned char> master_key = hex_to_vector(ui->auraKey->text());
    if(master_key.size() != AURA_Processor::KEY_SIZE) {
        aura_error(AURA_Result::Error_Invalid_Input);
        return false;
    }

    QImage image(job.currentFile);
    if(image.isNull()) {
        display_error("AURA Error", "Could not load the source image for AURA reveal.");
        return false;
    }
    image = image.convertToFormat(QImage::Format_ARGB32);
    AURA_ImageBuffer image_buffer { image.bits(), (size_t)image.width(), (size_t)image.height() };

    // call the aura library to perform extraction and decryption
    AURA_Processor aura(master_key);
    std::vector<unsigned char> decrypted_payload;
    AURA_Result result = aura.decrypt(decrypted_payload, image_buffer);

    if (result != AURA_Result::Success) {
        aura_error(result);
        return false;
    }

    // parse the decrypted payload to get original filename and content
    QByteArray payload_qba(reinterpret_cast<const char*>(decrypted_payload.data()), decrypted_payload.size());
    QDataStream stream(&payload_qba, QIODevice::ReadOnly);
    quint64 total_len;
    quint16 filename_len;
    stream >> total_len >> filename_len;
    QByteArray filename_qba;
    filename_qba.resize(filename_len);
    stream.readRawData(filename_qba.data(), filename_len);
    QString original_filename = QString::fromUtf8(filename_qba);
    QByteArray file_content = payload_qba.right(payload_qba.size() - stream.device()->pos());

    // save the revealed file
    QString final_path = job.outputFolder + original_filename;
    QFile revealed_file(final_path);
    if(!revealed_file.open(QIODevice::WriteOnly)) {
        display_error("Save Error", "Could not create the revealed output file.");
        return false;
    }
    revealed_file.write(file_content);
    revealed_file.close();

    if(job.currentFile != job.originalFile) {
        job.interimFiles.append(job.currentFile);
    }
    job.currentFile = final_path;
    return true;
}

//
// utility and helper functions
//

// image generation algorithm
QImage MainWindow::gen_img(qint64 required_pixels)
{
    if (required_pixels <= 0) {
        return QImage();
    }

    int width = static_cast<int>(std::sqrt(required_pixels));
    int height = (width > 0) ? ((required_pixels / width) + 1) : 1;

    QImage image(width, height, QImage::Format_ARGB32_Premultiplied);
    if (image.isNull()) {
        display_error("Memory Error", "Failed to allocate memory for generated image.");
        return QImage();
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distrib(0, 255);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            image.setPixel(x, y, qRgba(distrib(gen), distrib(gen), distrib(gen), 255));
        }
    }

    return image;
}

// converts a hex string from the ui to a byte vector for aura
std::vector<unsigned char> MainWindow::hex_to_vector(const QString& hex)
{
    QByteArray hex_data = hex.toLatin1();
    QByteArray bytes = QByteArray::fromHex(hex_data);
    return std::vector<unsigned char>(bytes.begin(), bytes.end());
}

// displays aura error message
void MainWindow::aura_error(AURA_Result result)
{
    QString title = "AURA Error";
    QString message = "An unknown error occurred during the AURA process.";

    switch (result) {
        case AURA_Result::Error_Authentication_Failed:
            message = "Authentication Failed. The master key is incorrect or the image file has been corrupted or modified.";
            break;
        case AURA_Result::Error_Image_Too_Small:
            message = "The selected cover image is too small to hold the data payload.";
            break;
        case AURA_Result::Error_Invalid_Input:
            message = "Invalid input provided to the AURA process. Check your key and file selections.";
            break;
        case AURA_Result::Error_Crypto_Error:
            message = "An internal cryptographic error occurred in the Botan library.";
            break;
        case AURA_Result::Success:
            return;
    }
    QMessageBox::critical(this, title, message);
}


//
// ui slots
//

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

// key generation slots
void MainWindow::on_aesGen_clicked()
{
    QCA::SymmetricKey key(32);
    QCA::InitializationVector iv(16);
    QString k(key.toByteArray().toHex().data());
    QString v(iv.toByteArray().toHex().data());
    QString kv = k + v;
    ui->aesKey->setText(kv);
    return;
}


void MainWindow::on_auraGen_clicked()
{
    QCA::SymmetricKey key(32);
    QString k(key.toByteArray().toHex().data());
    ui->auraKey->setText(k);
    return;
}



void MainWindow::on_simpGen_clicked()
{
    QCA::SymmetricKey key(8);
    QString k(key.toByteArray().toHex().data());
    ui->simpKey->setText(k);
    return;
}

// checkbox state management slots
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
