#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>
#include <QString>
#include <QList>
#include <memory>

// Use Botan for all cryptographic primatives
#include <botan/auto_rng.h>
#include <botan/hex.h>
#include <botan/pbkdf2.h>
#include <botan/cipher_mode.h>
#include <botan/secmem.h>

#include "AURA.h"
#include "fileformat.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

// manages the state of a file processing operation
struct ProcessingJob {
    QString originalFile;
    QString currentFile;
    QString outputFolder;
    QList<QString> interimFiles;
};

// holds the parsed contents of a .czip container file
struct CzipContainer {
    QList<CZipFormat::OperationCode> op_stack;
    QString original_filename;
    QByteArray payload;
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // ui slots for file Browse
    void on_inFileBrowse_clicked();
    void on_outFileBrowse_clicked();
    void on_slctImgBrowse_clicked();

    // ui slot for the main start button
    void on_start_clicked();

    // ui slots for key generation
    void on_aesGen_clicked();
    void on_auraGen_clicked();
    void on_simpGen_clicked();

    // ui slots for checkbox state management
    void on_compress_clicked();
    void on_decompress_clicked();
    void on_encrypt_clicked();
    void on_decrypt_clicked();
    void on_conceal_clicked();
    void on_reveal_clicked();

private:
    // job management helpers
    bool initialize_job(ProcessingJob& job);
    void cleanup_interim_files(ProcessingJob& job);
    void display_error(const QString& title, const QString& message);
    QList<CZipFormat::OperationCode> read_operation_stack(const QString& filePath);
    bool check_file_size(const QString& path);
    bool read_czip_container(ProcessingJob& job, CzipContainer& container);
    bool write_czip_container(ProcessingJob& job, const QByteArray& payload, QList<CZipFormat::OperationCode>& op_stack);
    QByteArray extract_raw_lsb_data(const QImage& image);
    bool process_extracted_czip_data(ProcessingJob& job, QByteArray& extractedData);
    QByteArray prepare_lsb_payload(CzipContainer& container);
    bool embed_raw_lsb_data(QImage& image, const QByteArray& data_to_embed);


    // forward processing functions
    bool compress(ProcessingJob& job);
    bool encrypt_simple(ProcessingJob& job);
    bool encrypt_aes(ProcessingJob& job);
    bool stego_lsb(ProcessingJob& job);
    bool aura_conceal(ProcessingJob& job);

    // reverse processing functions
    bool decompress(ProcessingJob& job);
    bool decrypt_simple(ProcessingJob& job);
    bool decrypt_aes(ProcessingJob& job);
    bool rev_lsb(ProcessingJob& job);
    bool aura_reveal(ProcessingJob& job);

    // utility functions
    QImage gen_img(qint64 required_pixels);
    std::vector<unsigned char> hex_to_vector(const QString& hex);
    void aura_error(AURA_Result result);
    QString generate_random_hex_key(size_t byte_length);

    Ui::MainWindow *ui;
    Botan::AutoSeeded_RNG m_rng;
};
#endif // MAINWINDOW_H
