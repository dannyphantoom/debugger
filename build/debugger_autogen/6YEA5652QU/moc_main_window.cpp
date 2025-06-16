/****************************************************************************
** Meta object code from reading C++ file 'main_window.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.17)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../include/main_window.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'main_window.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.17. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_debugger__DisassemblyView_t {
    QByteArrayData data[9];
    char stringdata0[172];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_debugger__DisassemblyView_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_debugger__DisassemblyView_t qt_meta_stringdata_debugger__DisassemblyView = {
    {
QT_MOC_LITERAL(0, 0, 25), // "debugger::DisassemblyView"
QT_MOC_LITERAL(1, 26, 22), // "address_double_clicked"
QT_MOC_LITERAL(2, 49, 0), // ""
QT_MOC_LITERAL(3, 50, 8), // "uint64_t"
QT_MOC_LITERAL(4, 59, 7), // "address"
QT_MOC_LITERAL(5, 67, 27), // "toggle_breakpoint_requested"
QT_MOC_LITERAL(6, 95, 23), // "go_to_address_requested"
QT_MOC_LITERAL(7, 119, 29), // "update_line_number_area_width"
QT_MOC_LITERAL(8, 149, 22) // "highlight_current_line"

    },
    "debugger::DisassemblyView\0"
    "address_double_clicked\0\0uint64_t\0"
    "address\0toggle_breakpoint_requested\0"
    "go_to_address_requested\0"
    "update_line_number_area_width\0"
    "highlight_current_line"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_debugger__DisassemblyView[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x06 /* Public */,
       5,    1,   42,    2, 0x06 /* Public */,
       6,    0,   45,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    0,   46,    2, 0x08 /* Private */,
       8,    0,   47,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void debugger::DisassemblyView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DisassemblyView *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->address_double_clicked((*reinterpret_cast< uint64_t(*)>(_a[1]))); break;
        case 1: _t->toggle_breakpoint_requested((*reinterpret_cast< uint64_t(*)>(_a[1]))); break;
        case 2: _t->go_to_address_requested(); break;
        case 3: _t->update_line_number_area_width(); break;
        case 4: _t->highlight_current_line(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (DisassemblyView::*)(uint64_t );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DisassemblyView::address_double_clicked)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (DisassemblyView::*)(uint64_t );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DisassemblyView::toggle_breakpoint_requested)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (DisassemblyView::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DisassemblyView::go_to_address_requested)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject debugger::DisassemblyView::staticMetaObject = { {
    QMetaObject::SuperData::link<QTextEdit::staticMetaObject>(),
    qt_meta_stringdata_debugger__DisassemblyView.data,
    qt_meta_data_debugger__DisassemblyView,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *debugger::DisassemblyView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *debugger::DisassemblyView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_debugger__DisassemblyView.stringdata0))
        return static_cast<void*>(this);
    return QTextEdit::qt_metacast(_clname);
}

int debugger::DisassemblyView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTextEdit::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void debugger::DisassemblyView::address_double_clicked(uint64_t _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void debugger::DisassemblyView::toggle_breakpoint_requested(uint64_t _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void debugger::DisassemblyView::go_to_address_requested()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
struct qt_meta_stringdata_debugger__DecompilerView_t {
    QByteArrayData data[4];
    char stringdata0[78];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_debugger__DecompilerView_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_debugger__DecompilerView_t qt_meta_stringdata_debugger__DecompilerView = {
    {
QT_MOC_LITERAL(0, 0, 24), // "debugger::DecompilerView"
QT_MOC_LITERAL(1, 25, 27), // "function_analysis_requested"
QT_MOC_LITERAL(2, 53, 0), // ""
QT_MOC_LITERAL(3, 54, 23) // "goto_assembly_requested"

    },
    "debugger::DecompilerView\0"
    "function_analysis_requested\0\0"
    "goto_assembly_requested"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_debugger__DecompilerView[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   24,    2, 0x06 /* Public */,
       3,    0,   25,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void debugger::DecompilerView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DecompilerView *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->function_analysis_requested(); break;
        case 1: _t->goto_assembly_requested(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (DecompilerView::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DecompilerView::function_analysis_requested)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (DecompilerView::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DecompilerView::goto_assembly_requested)) {
                *result = 1;
                return;
            }
        }
    }
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject debugger::DecompilerView::staticMetaObject = { {
    QMetaObject::SuperData::link<QTextEdit::staticMetaObject>(),
    qt_meta_stringdata_debugger__DecompilerView.data,
    qt_meta_data_debugger__DecompilerView,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *debugger::DecompilerView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *debugger::DecompilerView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_debugger__DecompilerView.stringdata0))
        return static_cast<void*>(this);
    return QTextEdit::qt_metacast(_clname);
}

int debugger::DecompilerView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTextEdit::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void debugger::DecompilerView::function_analysis_requested()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void debugger::DecompilerView::goto_assembly_requested()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
struct qt_meta_stringdata_debugger__MemoryView_t {
    QByteArrayData data[15];
    char stringdata0[191];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_debugger__MemoryView_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_debugger__MemoryView_t qt_meta_stringdata_debugger__MemoryView = {
    {
QT_MOC_LITERAL(0, 0, 20), // "debugger::MemoryView"
QT_MOC_LITERAL(1, 21, 24), // "memory_refresh_requested"
QT_MOC_LITERAL(2, 46, 0), // ""
QT_MOC_LITERAL(3, 47, 8), // "uint64_t"
QT_MOC_LITERAL(4, 56, 7), // "address"
QT_MOC_LITERAL(5, 64, 6), // "size_t"
QT_MOC_LITERAL(6, 71, 4), // "size"
QT_MOC_LITERAL(7, 76, 22), // "memory_write_requested"
QT_MOC_LITERAL(8, 99, 7), // "uint8_t"
QT_MOC_LITERAL(9, 107, 5), // "value"
QT_MOC_LITERAL(10, 113, 20), // "breakpoint_requested"
QT_MOC_LITERAL(11, 134, 15), // "on_item_changed"
QT_MOC_LITERAL(12, 150, 17), // "QTableWidgetItem*"
QT_MOC_LITERAL(13, 168, 4), // "item"
QT_MOC_LITERAL(14, 173, 17) // "show_go_to_dialog"

    },
    "debugger::MemoryView\0memory_refresh_requested\0"
    "\0uint64_t\0address\0size_t\0size\0"
    "memory_write_requested\0uint8_t\0value\0"
    "breakpoint_requested\0on_item_changed\0"
    "QTableWidgetItem*\0item\0show_go_to_dialog"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_debugger__MemoryView[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   39,    2, 0x06 /* Public */,
       7,    2,   44,    2, 0x06 /* Public */,
      10,    1,   49,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      11,    1,   52,    2, 0x08 /* Private */,
      14,    0,   55,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5,    4,    6,
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 8,    4,    9,
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 12,   13,
    QMetaType::Void,

       0        // eod
};

void debugger::MemoryView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MemoryView *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->memory_refresh_requested((*reinterpret_cast< uint64_t(*)>(_a[1])),(*reinterpret_cast< size_t(*)>(_a[2]))); break;
        case 1: _t->memory_write_requested((*reinterpret_cast< uint64_t(*)>(_a[1])),(*reinterpret_cast< uint8_t(*)>(_a[2]))); break;
        case 2: _t->breakpoint_requested((*reinterpret_cast< uint64_t(*)>(_a[1]))); break;
        case 3: _t->on_item_changed((*reinterpret_cast< QTableWidgetItem*(*)>(_a[1]))); break;
        case 4: _t->show_go_to_dialog(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (MemoryView::*)(uint64_t , size_t );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MemoryView::memory_refresh_requested)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (MemoryView::*)(uint64_t , uint8_t );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MemoryView::memory_write_requested)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (MemoryView::*)(uint64_t );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MemoryView::breakpoint_requested)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject debugger::MemoryView::staticMetaObject = { {
    QMetaObject::SuperData::link<QTableWidget::staticMetaObject>(),
    qt_meta_stringdata_debugger__MemoryView.data,
    qt_meta_data_debugger__MemoryView,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *debugger::MemoryView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *debugger::MemoryView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_debugger__MemoryView.stringdata0))
        return static_cast<void*>(this);
    return QTableWidget::qt_metacast(_clname);
}

int debugger::MemoryView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTableWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void debugger::MemoryView::memory_refresh_requested(uint64_t _t1, size_t _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void debugger::MemoryView::memory_write_requested(uint64_t _t1, uint8_t _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void debugger::MemoryView::breakpoint_requested(uint64_t _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
struct qt_meta_stringdata_debugger__RegistersView_t {
    QByteArrayData data[6];
    char stringdata0[94];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_debugger__RegistersView_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_debugger__RegistersView_t qt_meta_stringdata_debugger__RegistersView = {
    {
QT_MOC_LITERAL(0, 0, 23), // "debugger::RegistersView"
QT_MOC_LITERAL(1, 24, 29), // "navigate_to_address_requested"
QT_MOC_LITERAL(2, 54, 0), // ""
QT_MOC_LITERAL(3, 55, 8), // "uint64_t"
QT_MOC_LITERAL(4, 64, 7), // "address"
QT_MOC_LITERAL(5, 72, 21) // "view_memory_requested"

    },
    "debugger::RegistersView\0"
    "navigate_to_address_requested\0\0uint64_t\0"
    "address\0view_memory_requested"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_debugger__RegistersView[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   24,    2, 0x06 /* Public */,
       5,    1,   27,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,

       0        // eod
};

void debugger::RegistersView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<RegistersView *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->navigate_to_address_requested((*reinterpret_cast< uint64_t(*)>(_a[1]))); break;
        case 1: _t->view_memory_requested((*reinterpret_cast< uint64_t(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (RegistersView::*)(uint64_t );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RegistersView::navigate_to_address_requested)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (RegistersView::*)(uint64_t );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RegistersView::view_memory_requested)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject debugger::RegistersView::staticMetaObject = { {
    QMetaObject::SuperData::link<QTableWidget::staticMetaObject>(),
    qt_meta_stringdata_debugger__RegistersView.data,
    qt_meta_data_debugger__RegistersView,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *debugger::RegistersView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *debugger::RegistersView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_debugger__RegistersView.stringdata0))
        return static_cast<void*>(this);
    return QTableWidget::qt_metacast(_clname);
}

int debugger::RegistersView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTableWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void debugger::RegistersView::navigate_to_address_requested(uint64_t _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void debugger::RegistersView::view_memory_requested(uint64_t _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
struct qt_meta_stringdata_debugger__BreakpointView_t {
    QByteArrayData data[7];
    char stringdata0[129];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_debugger__BreakpointView_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_debugger__BreakpointView_t qt_meta_stringdata_debugger__BreakpointView = {
    {
QT_MOC_LITERAL(0, 0, 24), // "debugger::BreakpointView"
QT_MOC_LITERAL(1, 25, 27), // "breakpoint_toggle_requested"
QT_MOC_LITERAL(2, 53, 0), // ""
QT_MOC_LITERAL(3, 54, 8), // "uint64_t"
QT_MOC_LITERAL(4, 63, 7), // "address"
QT_MOC_LITERAL(5, 71, 27), // "breakpoint_remove_requested"
QT_MOC_LITERAL(6, 99, 29) // "navigate_to_address_requested"

    },
    "debugger::BreakpointView\0"
    "breakpoint_toggle_requested\0\0uint64_t\0"
    "address\0breakpoint_remove_requested\0"
    "navigate_to_address_requested"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_debugger__BreakpointView[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x06 /* Public */,
       5,    1,   32,    2, 0x06 /* Public */,
       6,    1,   35,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,

       0        // eod
};

void debugger::BreakpointView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<BreakpointView *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->breakpoint_toggle_requested((*reinterpret_cast< uint64_t(*)>(_a[1]))); break;
        case 1: _t->breakpoint_remove_requested((*reinterpret_cast< uint64_t(*)>(_a[1]))); break;
        case 2: _t->navigate_to_address_requested((*reinterpret_cast< uint64_t(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (BreakpointView::*)(uint64_t );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BreakpointView::breakpoint_toggle_requested)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (BreakpointView::*)(uint64_t );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BreakpointView::breakpoint_remove_requested)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (BreakpointView::*)(uint64_t );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BreakpointView::navigate_to_address_requested)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject debugger::BreakpointView::staticMetaObject = { {
    QMetaObject::SuperData::link<QTableWidget::staticMetaObject>(),
    qt_meta_stringdata_debugger__BreakpointView.data,
    qt_meta_data_debugger__BreakpointView,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *debugger::BreakpointView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *debugger::BreakpointView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_debugger__BreakpointView.stringdata0))
        return static_cast<void*>(this);
    return QTableWidget::qt_metacast(_clname);
}

int debugger::BreakpointView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTableWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void debugger::BreakpointView::breakpoint_toggle_requested(uint64_t _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void debugger::BreakpointView::breakpoint_remove_requested(uint64_t _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void debugger::BreakpointView::navigate_to_address_requested(uint64_t _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
struct qt_meta_stringdata_debugger__MainWindow_t {
    QByteArrayData data[28];
    char stringdata0[709];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_debugger__MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_debugger__MainWindow_t qt_meta_stringdata_debugger__MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 20), // "debugger::MainWindow"
QT_MOC_LITERAL(1, 21, 24), // "on_action_open_triggered"
QT_MOC_LITERAL(2, 46, 0), // ""
QT_MOC_LITERAL(3, 47, 25), // "on_action_close_triggered"
QT_MOC_LITERAL(4, 73, 24), // "on_action_exit_triggered"
QT_MOC_LITERAL(5, 98, 25), // "on_action_about_triggered"
QT_MOC_LITERAL(6, 124, 31), // "on_action_start_debug_triggered"
QT_MOC_LITERAL(7, 156, 34), // "on_action_attach_process_trig..."
QT_MOC_LITERAL(8, 191, 28), // "on_action_continue_triggered"
QT_MOC_LITERAL(9, 220, 25), // "on_action_pause_triggered"
QT_MOC_LITERAL(10, 246, 24), // "on_action_stop_triggered"
QT_MOC_LITERAL(11, 271, 29), // "on_action_step_into_triggered"
QT_MOC_LITERAL(12, 301, 29), // "on_action_step_over_triggered"
QT_MOC_LITERAL(13, 331, 28), // "on_action_step_out_triggered"
QT_MOC_LITERAL(14, 360, 33), // "on_action_go_to_address_trigg..."
QT_MOC_LITERAL(15, 394, 24), // "on_action_find_triggered"
QT_MOC_LITERAL(16, 419, 37), // "on_action_toggle_breakpoint_t..."
QT_MOC_LITERAL(17, 457, 37), // "on_action_analyze_functions_t..."
QT_MOC_LITERAL(18, 495, 32), // "on_action_show_strings_triggered"
QT_MOC_LITERAL(19, 528, 32), // "on_action_show_imports_triggered"
QT_MOC_LITERAL(20, 561, 32), // "on_action_show_exports_triggered"
QT_MOC_LITERAL(21, 594, 18), // "update_debug_state"
QT_MOC_LITERAL(22, 613, 17), // "on_breakpoint_hit"
QT_MOC_LITERAL(23, 631, 8), // "uint64_t"
QT_MOC_LITERAL(24, 640, 7), // "address"
QT_MOC_LITERAL(25, 648, 20), // "on_function_selected"
QT_MOC_LITERAL(26, 669, 25), // "on_address_double_clicked"
QT_MOC_LITERAL(27, 695, 13) // "refresh_views"

    },
    "debugger::MainWindow\0on_action_open_triggered\0"
    "\0on_action_close_triggered\0"
    "on_action_exit_triggered\0"
    "on_action_about_triggered\0"
    "on_action_start_debug_triggered\0"
    "on_action_attach_process_triggered\0"
    "on_action_continue_triggered\0"
    "on_action_pause_triggered\0"
    "on_action_stop_triggered\0"
    "on_action_step_into_triggered\0"
    "on_action_step_over_triggered\0"
    "on_action_step_out_triggered\0"
    "on_action_go_to_address_triggered\0"
    "on_action_find_triggered\0"
    "on_action_toggle_breakpoint_triggered\0"
    "on_action_analyze_functions_triggered\0"
    "on_action_show_strings_triggered\0"
    "on_action_show_imports_triggered\0"
    "on_action_show_exports_triggered\0"
    "update_debug_state\0on_breakpoint_hit\0"
    "uint64_t\0address\0on_function_selected\0"
    "on_address_double_clicked\0refresh_views"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_debugger__MainWindow[] = {

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
      15,    0,  147,    2, 0x08 /* Private */,
      16,    0,  148,    2, 0x08 /* Private */,
      17,    0,  149,    2, 0x08 /* Private */,
      18,    0,  150,    2, 0x08 /* Private */,
      19,    0,  151,    2, 0x08 /* Private */,
      20,    0,  152,    2, 0x08 /* Private */,
      21,    0,  153,    2, 0x08 /* Private */,
      22,    1,  154,    2, 0x08 /* Private */,
      25,    1,  157,    2, 0x08 /* Private */,
      26,    1,  160,    2, 0x08 /* Private */,
      27,    0,  163,    2, 0x08 /* Private */,

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
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 23,   24,
    QMetaType::Void, 0x80000000 | 23,   24,
    QMetaType::Void, 0x80000000 | 23,   24,
    QMetaType::Void,

       0        // eod
};

void debugger::MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->on_action_open_triggered(); break;
        case 1: _t->on_action_close_triggered(); break;
        case 2: _t->on_action_exit_triggered(); break;
        case 3: _t->on_action_about_triggered(); break;
        case 4: _t->on_action_start_debug_triggered(); break;
        case 5: _t->on_action_attach_process_triggered(); break;
        case 6: _t->on_action_continue_triggered(); break;
        case 7: _t->on_action_pause_triggered(); break;
        case 8: _t->on_action_stop_triggered(); break;
        case 9: _t->on_action_step_into_triggered(); break;
        case 10: _t->on_action_step_over_triggered(); break;
        case 11: _t->on_action_step_out_triggered(); break;
        case 12: _t->on_action_go_to_address_triggered(); break;
        case 13: _t->on_action_find_triggered(); break;
        case 14: _t->on_action_toggle_breakpoint_triggered(); break;
        case 15: _t->on_action_analyze_functions_triggered(); break;
        case 16: _t->on_action_show_strings_triggered(); break;
        case 17: _t->on_action_show_imports_triggered(); break;
        case 18: _t->on_action_show_exports_triggered(); break;
        case 19: _t->update_debug_state(); break;
        case 20: _t->on_breakpoint_hit((*reinterpret_cast< uint64_t(*)>(_a[1]))); break;
        case 21: _t->on_function_selected((*reinterpret_cast< uint64_t(*)>(_a[1]))); break;
        case 22: _t->on_address_double_clicked((*reinterpret_cast< uint64_t(*)>(_a[1]))); break;
        case 23: _t->refresh_views(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject debugger::MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_debugger__MainWindow.data,
    qt_meta_data_debugger__MainWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *debugger::MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *debugger::MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_debugger__MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int debugger::MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
