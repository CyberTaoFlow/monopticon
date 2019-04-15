/****************************************************************************
** Meta object code from reading C++ file 'capture_preferences_frame.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../ui/qt/capture_preferences_frame.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'capture_preferences_frame.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CapturePreferencesFrame_t {
    QByteArrayData data[10];
    char stringdata0[262];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CapturePreferencesFrame_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CapturePreferencesFrame_t qt_meta_stringdata_CapturePreferencesFrame = {
    {
QT_MOC_LITERAL(0, 0, 23), // "CapturePreferencesFrame"
QT_MOC_LITERAL(1, 24, 43), // "on_defaultInterfaceComboBox_e..."
QT_MOC_LITERAL(2, 68, 0), // ""
QT_MOC_LITERAL(3, 69, 9), // "new_iface"
QT_MOC_LITERAL(4, 79, 34), // "on_capturePromModeCheckBox_to..."
QT_MOC_LITERAL(5, 114, 7), // "checked"
QT_MOC_LITERAL(6, 122, 32), // "on_capturePcapNgCheckBox_toggled"
QT_MOC_LITERAL(7, 155, 34), // "on_captureRealTimeCheckBox_to..."
QT_MOC_LITERAL(8, 190, 36), // "on_captureAutoScrollCheckBox_..."
QT_MOC_LITERAL(9, 227, 34) // "on_captureNoExtcapCheckBox_to..."

    },
    "CapturePreferencesFrame\0"
    "on_defaultInterfaceComboBox_editTextChanged\0"
    "\0new_iface\0on_capturePromModeCheckBox_toggled\0"
    "checked\0on_capturePcapNgCheckBox_toggled\0"
    "on_captureRealTimeCheckBox_toggled\0"
    "on_captureAutoScrollCheckBox_toggled\0"
    "on_captureNoExtcapCheckBox_toggled"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CapturePreferencesFrame[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x08 /* Private */,
       4,    1,   47,    2, 0x08 /* Private */,
       6,    1,   50,    2, 0x08 /* Private */,
       7,    1,   53,    2, 0x08 /* Private */,
       8,    1,   56,    2, 0x08 /* Private */,
       9,    1,   59,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::Bool,    5,
    QMetaType::Void, QMetaType::Bool,    5,
    QMetaType::Void, QMetaType::Bool,    5,
    QMetaType::Void, QMetaType::Bool,    5,
    QMetaType::Void, QMetaType::Bool,    5,

       0        // eod
};

void CapturePreferencesFrame::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CapturePreferencesFrame *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_defaultInterfaceComboBox_editTextChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->on_capturePromModeCheckBox_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->on_capturePcapNgCheckBox_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->on_captureRealTimeCheckBox_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->on_captureAutoScrollCheckBox_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->on_captureNoExtcapCheckBox_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CapturePreferencesFrame::staticMetaObject = { {
    &QFrame::staticMetaObject,
    qt_meta_stringdata_CapturePreferencesFrame.data,
    qt_meta_data_CapturePreferencesFrame,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CapturePreferencesFrame::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CapturePreferencesFrame::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CapturePreferencesFrame.stringdata0))
        return static_cast<void*>(this);
    return QFrame::qt_metacast(_clname);
}

int CapturePreferencesFrame::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
