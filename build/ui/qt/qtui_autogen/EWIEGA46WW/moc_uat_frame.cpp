/****************************************************************************
** Meta object code from reading C++ file 'uat_frame.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../ui/qt/uat_frame.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'uat_frame.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_UatFrame_t {
    QByteArrayData data[16];
    char stringdata0[294];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_UatFrame_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_UatFrame_t qt_meta_stringdata_UatFrame = {
    {
QT_MOC_LITERAL(0, 0, 8), // "UatFrame"
QT_MOC_LITERAL(1, 9, 16), // "modelDataChanged"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 11), // "QModelIndex"
QT_MOC_LITERAL(4, 39, 7), // "topLeft"
QT_MOC_LITERAL(5, 47, 16), // "modelRowsRemoved"
QT_MOC_LITERAL(6, 64, 14), // "modelRowsReset"
QT_MOC_LITERAL(7, 79, 33), // "on_uatTreeView_currentItemCha..."
QT_MOC_LITERAL(8, 113, 7), // "current"
QT_MOC_LITERAL(9, 121, 8), // "previous"
QT_MOC_LITERAL(10, 130, 24), // "on_newToolButton_clicked"
QT_MOC_LITERAL(11, 155, 27), // "on_deleteToolButton_clicked"
QT_MOC_LITERAL(12, 183, 25), // "on_copyToolButton_clicked"
QT_MOC_LITERAL(13, 209, 27), // "on_moveUpToolButton_clicked"
QT_MOC_LITERAL(14, 237, 29), // "on_moveDownToolButton_clicked"
QT_MOC_LITERAL(15, 267, 26) // "on_clearToolButton_clicked"

    },
    "UatFrame\0modelDataChanged\0\0QModelIndex\0"
    "topLeft\0modelRowsRemoved\0modelRowsReset\0"
    "on_uatTreeView_currentItemChanged\0"
    "current\0previous\0on_newToolButton_clicked\0"
    "on_deleteToolButton_clicked\0"
    "on_copyToolButton_clicked\0"
    "on_moveUpToolButton_clicked\0"
    "on_moveDownToolButton_clicked\0"
    "on_clearToolButton_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_UatFrame[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   64,    2, 0x08 /* Private */,
       5,    0,   67,    2, 0x08 /* Private */,
       6,    0,   68,    2, 0x08 /* Private */,
       7,    2,   69,    2, 0x08 /* Private */,
      10,    0,   74,    2, 0x08 /* Private */,
      11,    0,   75,    2, 0x08 /* Private */,
      12,    0,   76,    2, 0x08 /* Private */,
      13,    0,   77,    2, 0x08 /* Private */,
      14,    0,   78,    2, 0x08 /* Private */,
      15,    0,   79,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3,    8,    9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void UatFrame::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<UatFrame *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->modelDataChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 1: _t->modelRowsRemoved(); break;
        case 2: _t->modelRowsReset(); break;
        case 3: _t->on_uatTreeView_currentItemChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< const QModelIndex(*)>(_a[2]))); break;
        case 4: _t->on_newToolButton_clicked(); break;
        case 5: _t->on_deleteToolButton_clicked(); break;
        case 6: _t->on_copyToolButton_clicked(); break;
        case 7: _t->on_moveUpToolButton_clicked(); break;
        case 8: _t->on_moveDownToolButton_clicked(); break;
        case 9: _t->on_clearToolButton_clicked(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject UatFrame::staticMetaObject = { {
    &QFrame::staticMetaObject,
    qt_meta_stringdata_UatFrame.data,
    qt_meta_data_UatFrame,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *UatFrame::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *UatFrame::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_UatFrame.stringdata0))
        return static_cast<void*>(this);
    return QFrame::qt_metacast(_clname);
}

int UatFrame::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
