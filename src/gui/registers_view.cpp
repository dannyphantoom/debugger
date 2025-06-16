#include "main_window.h"
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTableWidgetItem>
#include <QtWidgets/QMenu>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtGui/QFont>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QClipboard>
#include <QtCore/QVariant>

namespace debugger {

RegistersView::RegistersView(QWidget* parent) : QTableWidget(parent) {
    setup_table();
    setup_columns();
}

void RegistersView::setup_table() {
    setColumnCount(3);
    setHorizontalHeaderLabels({"Register", "Value", "Size"});
    setAlternatingRowColors(true);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    // Set font to monospace for consistent formatting
    QFont mono_font("Consolas", 10);
    mono_font.setStyleHint(QFont::Monospace);
    setFont(mono_font);
    
    // Configure headers
    horizontalHeader()->setStretchLastSection(false);
    horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    
    verticalHeader()->setVisible(false);
    
    // Set up default register set (x86-64)
    initialize_default_registers();
}

void RegistersView::setup_columns() {
    setColumnCount(3);
    setHorizontalHeaderLabels({"Register", "Value", "Size"});
}

void RegistersView::initialize_default_registers() {
    // x86-64 general purpose registers
    std::vector<std::string> default_registers = {
        "RAX", "RBX", "RCX", "RDX", "RSI", "RDI", "RSP", "RBP",
        "R8", "R9", "R10", "R11", "R12", "R13", "R14", "R15",
        "RIP", "RFLAGS", "CS", "DS", "ES", "FS", "GS", "SS"
    };
    
    setRowCount(default_registers.size());
    
    for (size_t i = 0; i < default_registers.size(); ++i) {
        // Register name
        QTableWidgetItem* name_item = new QTableWidgetItem(QString::fromStdString(default_registers[i]));
        name_item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        setItem(i, 0, name_item);
        
        // Value (initially unknown)
        QTableWidgetItem* value_item = new QTableWidgetItem("0x0000000000000000");
        value_item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        setItem(i, 1, value_item);
        
        // Size
        QTableWidgetItem* size_item = new QTableWidgetItem("8");
        size_item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        setItem(i, 2, size_item);
    }
    
    resizeColumnsToContents();
}

void RegistersView::set_registers(const std::vector<Register>& registers) {
    // Store previous values for comparison
    std::map<std::string, uint64_t> old_values;
    for (const auto& reg : previous_registers) {
        old_values[reg.name] = reg.value;
    }
    
    // Clear existing items
    setRowCount(0);
    
    if (registers.empty()) {
        initialize_default_registers();
        return;
    }
    
    setRowCount(registers.size());
    
    for (size_t i = 0; i < registers.size(); ++i) {
        const auto& reg = registers[i];
        
        // Register name
        QTableWidgetItem* name_item = new QTableWidgetItem(QString::fromStdString(reg.name));
        name_item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        setItem(i, 0, name_item);
        
        // Format value based on size
        QString value_str;
        switch (reg.size) {
            case 1:
                value_str = QString("0x%1").arg(reg.value & 0xFF, 2, 16, QChar('0')).toUpper();
                break;
            case 2:
                value_str = QString("0x%1").arg(reg.value & 0xFFFF, 4, 16, QChar('0')).toUpper();
                break;
            case 4:
                value_str = QString("0x%1").arg(reg.value & 0xFFFFFFFF, 8, 16, QChar('0')).toUpper();
                break;
            case 8:
            default:
                value_str = QString("0x%1").arg(reg.value, 16, 16, QChar('0')).toUpper();
                break;
        }
        
        QTableWidgetItem* value_item = new QTableWidgetItem(value_str);
        value_item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        
        // Highlight modified registers
        auto old_it = old_values.find(reg.name);
        if (old_it != old_values.end() && old_it->second != reg.value) {
            // Register value changed - highlight it
            value_item->setBackground(QColor(255, 255, 0, 100)); // Light yellow
            value_item->setToolTip(QString("Changed from: 0x%1").arg(old_it->second, 16, 16, QChar('0')).toUpper());
        } else if (reg.modified) {
            value_item->setBackground(QColor(255, 165, 0, 100)); // Light orange
            value_item->setToolTip("Register was modified");
        }
        
        setItem(i, 1, value_item);
        
        // Size
        QTableWidgetItem* size_item = new QTableWidgetItem(QString::number(reg.size));
        size_item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        setItem(i, 2, size_item);
        
        // Add additional info as tooltip
        QString tooltip = QString("Register: %1\nValue: %2\nSize: %3 bytes")
                         .arg(QString::fromStdString(reg.name))
                         .arg(value_str)
                         .arg(reg.size);
        
        if (reg.name == "RIP" || reg.name == "EIP") {
            tooltip += "\n(Instruction Pointer)";
        } else if (reg.name == "RSP" || reg.name == "ESP") {
            tooltip += "\n(Stack Pointer)";
        } else if (reg.name == "RBP" || reg.name == "EBP") {
            tooltip += "\n(Base Pointer)";
        } else if (reg.name == "RFLAGS" || reg.name == "EFLAGS") {
            tooltip += "\n(Flags Register)";
            tooltip += format_flags_register(reg.value);
        }
        
        name_item->setToolTip(tooltip);
        value_item->setToolTip(tooltip);
    }
    
    previous_registers = registers;
    resizeColumnsToContents();
}

QString RegistersView::format_flags_register(uint64_t flags) {
    QString flags_str = "\nFlags: ";
    
    // Common x86 flags
    if (flags & (1 << 0)) flags_str += "CF ";   // Carry Flag
    if (flags & (1 << 2)) flags_str += "PF ";   // Parity Flag
    if (flags & (1 << 4)) flags_str += "AF ";   // Auxiliary Flag
    if (flags & (1 << 6)) flags_str += "ZF ";   // Zero Flag
    if (flags & (1 << 7)) flags_str += "SF ";   // Sign Flag
    if (flags & (1 << 8)) flags_str += "TF ";   // Trap Flag
    if (flags & (1 << 9)) flags_str += "IF ";   // Interrupt Flag
    if (flags & (1 << 10)) flags_str += "DF ";  // Direction Flag
    if (flags & (1 << 11)) flags_str += "OF ";  // Overflow Flag
    
    return flags_str;
}

void RegistersView::highlight_modified_registers() {
    // This is called after set_registers, highlighting is already done there
    update();
}

void RegistersView::clear_highlights() {
    for (int i = 0; i < rowCount(); ++i) {
        for (int j = 0; j < columnCount(); ++j) {
            QTableWidgetItem* item = this->item(i, j);
            if (item) {
                item->setBackground(QBrush());
                item->setToolTip("");
            }
        }
    }
}

void RegistersView::contextMenuEvent(QContextMenuEvent* event) {
    QTableWidgetItem* item = itemAt(event->pos());
    if (!item) {
        return;
    }
    
    int row = item->row();
    QString register_name = this->item(row, 0)->text();
    QString register_value = this->item(row, 1)->text();
    
    QMenu menu(this);
    
    QAction* copy_name = menu.addAction(QString("Copy Register Name (%1)").arg(register_name));
    connect(copy_name, &QAction::triggered, [register_name] {
        QApplication::clipboard()->setText(register_name);
    });
    
    QAction* copy_value = menu.addAction(QString("Copy Value (%1)").arg(register_value));
    connect(copy_value, &QAction::triggered, [register_value] {
        QApplication::clipboard()->setText(register_value);
    });
    
    menu.addSeparator();
    
    // Add actions based on register type
    if (register_name == "RIP" || register_name == "EIP") {
        QAction* goto_ip = menu.addAction("Go to Instruction Pointer");
        connect(goto_ip, &QAction::triggered, [this, register_value] {
            emit navigate_to_address_requested(register_value.mid(2).toULongLong(nullptr, 16));
        });
    } else if (register_name == "RSP" || register_name == "ESP") {
        QAction* view_stack = menu.addAction("View Stack");
        connect(view_stack, &QAction::triggered, [this, register_value] {
            emit view_memory_requested(register_value.mid(2).toULongLong(nullptr, 16));
        });
    }
    
    // Add modify register action (for future implementation)
    QAction* modify = menu.addAction("Modify Register...");
    modify->setEnabled(false); // TODO: Implement register modification
    
    menu.exec(event->globalPos());
}

// Helper function to convert BreakpointType enum to string
QString breakpoint_type_to_string(BreakpointType type) {
    switch (type) {
        case BreakpointType::SOFTWARE:
            return "Software";
        case BreakpointType::HARDWARE:
            return "Hardware";
        case BreakpointType::CONDITIONAL:
            return "Conditional";
        default:
            return "Unknown";
    }
}

// BreakpointView implementation
BreakpointView::BreakpointView(QWidget* parent) : QTableWidget(parent) {
    setup_table();
    setup_columns();
}

void BreakpointView::setup_table() {
    setColumnCount(4);
    setHorizontalHeaderLabels({"Address", "Type", "Enabled", "Condition"});
    setAlternatingRowColors(true);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    // Set font to monospace for consistent formatting
    QFont mono_font("Consolas", 10);
    mono_font.setStyleHint(QFont::Monospace);
    setFont(mono_font);
    
    // Configure headers
    horizontalHeader()->setStretchLastSection(true);
    horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    
    verticalHeader()->setVisible(false);
}

void BreakpointView::setup_columns() {
    setColumnCount(4);
    setHorizontalHeaderLabels({"Address", "Type", "Enabled", "Condition"});
}

void BreakpointView::set_breakpoints(const std::vector<Breakpoint>& breakpoints) {
    current_breakpoints = breakpoints;
    
    // Clear existing items
    setRowCount(0);
    
    if (breakpoints.empty()) {
        return;
    }
    
    setRowCount(breakpoints.size());
    
    for (size_t i = 0; i < breakpoints.size(); ++i) {
        const auto& bp = breakpoints[i];
        
        // Address
        QTableWidgetItem* addr_item = new QTableWidgetItem(
            QString("0x%1").arg(bp.address, 16, 16, QChar('0')).toUpper()
        );
        addr_item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        setItem(i, 0, addr_item);
        
        // Type
        QTableWidgetItem* type_item = new QTableWidgetItem(breakpoint_type_to_string(bp.type));
        type_item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        setItem(i, 1, type_item);
        
        // Enabled
        QTableWidgetItem* enabled_item = new QTableWidgetItem(bp.enabled ? "Yes" : "No");
        enabled_item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        if (bp.enabled) {
            enabled_item->setForeground(QColor(0, 128, 0)); // Green for enabled
        } else {
            enabled_item->setForeground(QColor(128, 128, 128)); // Gray for disabled
        }
        setItem(i, 2, enabled_item);
        
        // Condition
        QTableWidgetItem* condition_item = new QTableWidgetItem(QString::fromStdString(bp.condition));
        condition_item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        setItem(i, 3, condition_item);
        
        // Add tooltip with full information
        QString tooltip = QString("Breakpoint at 0x%1\nType: %2\nEnabled: %3\nHit count: %4")
                         .arg(bp.address, 16, 16, QChar('0'))
                         .arg(breakpoint_type_to_string(bp.type))
                         .arg(bp.enabled ? "Yes" : "No")
                         .arg(bp.hit_count);
        
        if (!bp.condition.empty()) {
            tooltip += QString("\nCondition: %1").arg(QString::fromStdString(bp.condition));
        }
        
        addr_item->setToolTip(tooltip);
    }
    
    resizeColumnsToContents();
}

void BreakpointView::add_breakpoint(const Breakpoint& bp) {
    current_breakpoints.push_back(bp);
    set_breakpoints(current_breakpoints);
}

void BreakpointView::remove_breakpoint(uint64_t address) {
    current_breakpoints.erase(
        std::remove_if(current_breakpoints.begin(), current_breakpoints.end(),
                      [address](const Breakpoint& bp) {
                          return bp.address == address;
                      }),
        current_breakpoints.end());
    set_breakpoints(current_breakpoints);
}

void BreakpointView::update_breakpoint_list() {
    // Refresh the current display
    set_breakpoints(current_breakpoints);
}

void BreakpointView::contextMenuEvent(QContextMenuEvent* event) {
    QTableWidgetItem* item = itemAt(event->pos());
    if (!item) {
        return;
    }
    
    int row = item->row();
    if (row < 0 || row >= static_cast<int>(current_breakpoints.size())) {
        return;
    }
    
    const Breakpoint& bp = current_breakpoints[row];
    
    QMenu menu(this);
    
    // Copy address
    QAction* copy_address = menu.addAction(QString("Copy Address (0x%1)")
                                         .arg(bp.address, 16, 16, QChar('0')).toUpper());
    connect(copy_address, &QAction::triggered, [bp]() {
        QApplication::clipboard()->setText(QString("0x%1").arg(bp.address, 16, 16, QChar('0')).toUpper());
    });
    
    menu.addSeparator();
    
    // Enable/Disable breakpoint
    QString enable_text = bp.enabled ? "Disable Breakpoint" : "Enable Breakpoint";
    QAction* toggle_enabled = menu.addAction(enable_text);
    connect(toggle_enabled, &QAction::triggered, [this, bp]() {
        emit breakpoint_toggle_requested(bp.address);
    });
    
    // Remove breakpoint
    QAction* remove_bp = menu.addAction("Remove Breakpoint");
    connect(remove_bp, &QAction::triggered, [this, bp]() {
        emit breakpoint_remove_requested(bp.address);
    });
    
    menu.addSeparator();
    
    // Go to address
    QAction* goto_address = menu.addAction("Go to Address");
    connect(goto_address, &QAction::triggered, [this, bp]() {
        emit navigate_to_address_requested(bp.address);
    });
    
    menu.exec(event->globalPos());
}

} // namespace debugger 