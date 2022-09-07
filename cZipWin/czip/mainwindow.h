#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_inFileBrowse_clicked();

    void on_outFileBrowse_clicked();

    void on_slctImgBrowse_clicked();

    void on_start_clicked();

    void on_aesGen_clicked();

    void on_seededGen_clicked();

    void on_simpGen_clicked();

    void on_compress_clicked();

    void on_decompress_clicked();

    void on_encrypt_clicked();

    void on_decrypt_clicked();

    void on_conceal_clicked();

    void on_reveal_clicked();

    void compress(std::string&);

    void encrypt_simple(std::string&);

    void encrypt_aes(std::string&);

    void gen_img(QImage&, int);

    void stego_lsb(std::string&);

    void stego_seed(std::string&);

    void rev_lsb(std::string&);

    void rev_seed(std::string&);

    void decrypt_simple(std::string);

    void decrypt_aes(std::string&);

    void decompress(std::string&);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
