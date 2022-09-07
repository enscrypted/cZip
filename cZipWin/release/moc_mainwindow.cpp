/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../czip/mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[29];
    char stringdata0[431];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 23), // "on_inFileBrowse_clicked"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 24), // "on_outFileBrowse_clicked"
QT_MOC_LITERAL(4, 61, 24), // "on_slctImgBrowse_clicked"
QT_MOC_LITERAL(5, 86, 16), // "on_start_clicked"
QT_MOC_LITERAL(6, 103, 17), // "on_aesGen_clicked"
QT_MOC_LITERAL(7, 121, 20), // "on_seededGen_clicked"
QT_MOC_LITERAL(8, 142, 18), // "on_simpGen_clicked"
QT_MOC_LITERAL(9, 161, 19), // "on_compress_clicked"
QT_MOC_LITERAL(10, 181, 21), // "on_decompress_clicked"
QT_MOC_LITERAL(11, 203, 18), // "on_encrypt_clicked"
QT_MOC_LITERAL(12, 222, 18), // "on_decrypt_clicked"
QT_MOC_LITERAL(13, 241, 18), // "on_conceal_clicked"
QT_MOC_LITERAL(14, 260, 17), // "on_reveal_clicked"
QT_MOC_LITERAL(15, 278, 8), // "compress"
QT_MOC_LITERAL(16, 287, 12), // "std::string&"
QT_MOC_LITERAL(17, 300, 14), // "encrypt_simple"
QT_MOC_LITERAL(18, 315, 11), // "encrypt_aes"
QT_MOC_LITERAL(19, 327, 7), // "gen_img"
QT_MOC_LITERAL(20, 335, 7), // "QImage&"
QT_MOC_LITERAL(21, 343, 9), // "stego_lsb"
QT_MOC_LITERAL(22, 353, 10), // "stego_seed"
QT_MOC_LITERAL(23, 364, 7), // "rev_lsb"
QT_MOC_LITERAL(24, 372, 8), // "rev_seed"
QT_MOC_LITERAL(25, 381, 14), // "decrypt_simple"
QT_MOC_LITERAL(26, 396, 11), // "std::string"
QT_MOC_LITERAL(27, 408, 11), // "decrypt_aes"
QT_MOC_LITERAL(28, 420, 10) // "decompress"

    },
    "MainWindow\0on_inFileBrowse_clicked\0\0"
    "on_outFileBrowse_clicked\0"
    "on_slctImgBrowse_clicked\0on_start_clicked\0"
    "on_aesGen_clicked\0on_seededGen_clicked\0"
    "on_simpGen_clicked\0on_compress_clicked\0"
    "on_decompress_clicked\0on_encrypt_clicked\0"
    "on_decrypt_clicked\0on_conceal_clicked\0"
    "on_reveal_clicked\0compress\0std::string&\0"
    "encrypt_simple\0encrypt_aes\0gen_img\0"
    "QImage&\0stego_lsb\0stego_seed\0rev_lsb\0"
    "rev_seed\0decrypt_simple\0std::string\0"
    "decrypt_aes\0decompress"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      24,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  134,    2, 0x08 /* Private */,
       3,    0,  135,    2, 0x08 /* Private */,
       4,    0,  136,    2, 0x08 /* Private */,
       5,    0,  137,    2, 0x08 /* Private */,
       6,    0,  138,    2, 0x08 /* Private */,
       7,    0,  139,    2, 0x08 /* Private */,
       8,    0,  140,    2, 0x08 /* Private */,
       9,    0,  141,    2, 0x08 /* Private */,
      10,    0,  142,    2, 0x08 /* Private */,
      11,    0,  143,    2, 0x08 /* Private */,
      12,    0,  144,    2, 0x08 /* Private */,
      13,    0,  145,    2, 0x08 /* Private */,
      14,    0,  146,    2, 0x08 /* Private */,
      15,    1,  147,    2, 0x08 /* Private */,
      17,    1,  150,    2, 0x08 /* Private */,
      18,    1,  153,    2, 0x08 /* Private */,
      19,    2,  156,    2, 0x08 /* Private */,
      21,    1,  161,    2, 0x08 /* Private */,
      22,    1,  164,    2, 0x08 /* Private */,
      23,    1,  167,    2, 0x08 /* Private */,
      24,    1,  170,    2, 0x08 /* Private */,
      25,    1,  173,    2, 0x08 /* Private */,
      27,    1,  176,    2, 0x08 /* Private */,
      28,    1,  179,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 16,    2,
    QMetaType::Void, 0x80000000 | 16,    2,
    QMetaType::Void, 0x80000000 | 16,    2,
    QMetaType::Void, 0x80000000 | 20, QMetaType::Int,    2,    2,
    QMetaType::Void, 0x80000000 | 16,    2,
    QMetaType::Void, 0x80000000 | 16,    2,
    QMetaType::Void, 0x80000000 | 16,    2,
    QMetaType::Void, 0x80000000 | 16,    2,
    QMetaType::Void, 0x80000000 | 26,    2,
    QMetaType::Void, 0x80000000 | 16,    2,
    QMetaType::Void, 0x80000000 | 16,    2,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_inFileBrowse_clicked(); break;
        case 1: _t->on_outFileBrowse_clicked(); break;
        case 2: _t->on_slctImgBrowse_clicked(); break;
        case 3: _t->on_start_clicked(); break;
        case 4: _t->on_aesGen_clicked(); break;
        case 5: _t->on_seededGen_clicked(); break;
        case 6: _t->on_simpGen_clicked(); break;
        case 7: _t->on_compress_clicked(); break;
        case 8: _t->on_decompress_clicked(); break;
        case 9: _t->on_encrypt_clicked(); break;
        case 10: _t->on_decrypt_clicked(); break;
        case 11: _t->on_conceal_clicked(); break;
        case 12: _t->on_reveal_clicked(); break;
        case 13: _t->compress((*reinterpret_cast< std::string(*)>(_a[1]))); break;
        case 14: _t->encrypt_simple((*reinterpret_cast< std::string(*)>(_a[1]))); break;
        case 15: _t->encrypt_aes((*reinterpret_cast< std::string(*)>(_a[1]))); break;
        case 16: _t->gen_img((*reinterpret_cast< QImage(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 17: _t->stego_lsb((*reinterpret_cast< std::string(*)>(_a[1]))); break;
        case 18: _t->stego_seed((*reinterpret_cast< std::string(*)>(_a[1]))); break;
        case 19: _t->rev_lsb((*reinterpret_cast< std::string(*)>(_a[1]))); break;
        case 20: _t->rev_seed((*reinterpret_cast< std::string(*)>(_a[1]))); break;
        case 21: _t->decrypt_simple((*reinterpret_cast< std::string(*)>(_a[1]))); break;
        case 22: _t->decrypt_aes((*reinterpret_cast< std::string(*)>(_a[1]))); break;
        case 23: _t->decompress((*reinterpret_cast< std::string(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_MainWindow.data,
    qt_meta_data_MainWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 24)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 24;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 24)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 24;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
