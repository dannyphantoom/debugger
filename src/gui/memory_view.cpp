#include "main_window.h"
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTableWidgetItem>
#include <QtWidgets/QMenu>
#include <QtWidgets/QAction>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMessageBox>
#include <QtGui/QFont>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QClipboard>
#include <QtCore/QStringList>

namespace debugger {

MemoryView::MemoryView(QWidget* parent) 
    : QTableWidget(parent), base_address(0), bytes_per_row(16) {
    setup_table();
}

void MemoryView::setup_table() {
    // Set up columns: Address + 16 hex bytes + ASCII
    setColumnCount(18); // Address + 16 bytes + ASCII
    
    QStringList headers;
    headers << "Address";
    
    // Hex byte columns
    for (int i = 0; i < bytes_per_row; ++i) {
        headers << QString("%1").arg(i, 2, 16, QChar('0')).toUpper();
    }
    
    // ASCII column
    headers << "ASCII";
    
    setHorizontalHeaderLabels(headers);
    
    // Configure table properties
    setAlternatingRowColors(true);
    setSelectionBehavior(QAbstractItemView::SelectItems);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    
    // Set monospace font
    QFont mono_font("Consolas", 10);
    mono_font.setStyleHint(QFont::Monospace);
    setFont(mono_font);
    
    // Configure column widths
    horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents); // Address
    
    // Hex columns - fixed width
    for (int i = 1; i <= bytes_per_row; ++i) {
        horizontalHeader()->setSectionResizeMode(i, QHeaderView::Fixed);
        setColumnWidth(i, 30);
    }
    
    // ASCII column
    horizontalHeader()->setSectionResizeMode(17, QHeaderView::ResizeToContents);
    
    verticalHeader()->setVisible(false);
    
    // Enable editing for hex bytes
    setEditTriggers(QAbstractItemView::DoubleClicked);
    
    // Connect item changed signal for memory editing
    connect(this, &QTableWidget::itemChanged, this, &MemoryView::on_item_changed);
}

void MemoryView::set_memory_data(uint64_t start_address, const std::vector<uint8_t>& data) {
    base_address = start_address;
    memory_data = data;
    update_display();
}

void MemoryView::update_display() {
    if (memory_data.empty()) {
        setRowCount(0);
        return;
    }
    
    // Calculate number of rows needed
    int rows = (memory_data.size() + bytes_per_row - 1) / bytes_per_row;
    setRowCount(rows);
    
    for (int row = 0; row < rows; ++row) {
        uint64_t row_address = base_address + (row * bytes_per_row);
        
        // Address column
        QTableWidgetItem* addr_item = new QTableWidgetItem(
            QString("0x%1").arg(row_address, 16, 16, QChar('0')).toUpper()
        );
        addr_item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        addr_item->setBackground(QColor(240, 240, 240));
        setItem(row, 0, addr_item);
        
        // Hex byte columns and build ASCII string
        QString ascii_str;
        for (int col = 0; col < bytes_per_row; ++col) {
            int data_index = row * bytes_per_row + col;
            
            if (data_index < static_cast<int>(memory_data.size())) {
                uint8_t byte_value = memory_data[data_index];
                
                // Hex representation
                QTableWidgetItem* hex_item = new QTableWidgetItem(
                    QString("%1").arg(byte_value, 2, 16, QChar('0')).toUpper()
                );
                hex_item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
                hex_item->setTextAlignment(Qt::AlignCenter);
                
                // Color code based on value
                if (byte_value == 0x00) {
                    hex_item->setForeground(QColor(128, 128, 128)); // Gray for null bytes
                } else if (byte_value == 0xFF) {
                    hex_item->setForeground(QColor(255, 0, 0)); // Red for 0xFF
                } else if (std::isprint(byte_value)) {
                    hex_item->setForeground(QColor(0, 128, 0)); // Green for printable
                }
                
                setItem(row, col + 1, hex_item);
                
                // ASCII representation
                if (std::isprint(byte_value) && byte_value >= 32) {
                    ascii_str += static_cast<char>(byte_value);
                } else {
                    ascii_str += '.';
                }
            } else {
                // Empty cell for data beyond available
                QTableWidgetItem* empty_item = new QTableWidgetItem("");
                empty_item->setFlags(Qt::ItemIsEnabled);
                empty_item->setBackground(QColor(245, 245, 245));
                setItem(row, col + 1, empty_item);
                ascii_str += ' ';
            }
        }
        
        // ASCII column
        QTableWidgetItem* ascii_item = new QTableWidgetItem(ascii_str);
        ascii_item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        ascii_item->setFont(mono_font);
        setItem(row, 17, ascii_item);
    }
    
    resizeColumnsToContents();
}

void MemoryView::refresh_memory(uint64_t address, size_t size) {
    // Emit signal to request memory refresh from debugger
    emit memory_refresh_requested(address, size);
}

void MemoryView::navigate_to_address(uint64_t address) {
    // Find the row containing this address
    if (address < base_address) {
        // Address is before current view
        emit memory_refresh_requested(address, memory_data.size());
        return;
    }
    
    uint64_t offset = address - base_address;
    if (offset >= memory_data.size()) {
        // Address is after current view
        emit memory_refresh_requested(address, memory_data.size());
        return;
    }
    
    // Calculate row and column
    int row = offset / bytes_per_row;
    int col = (offset % bytes_per_row) + 1; // +1 for address column
    
    // Select and scroll to the byte
    setCurrentCell(row, col);
    scrollToItem(item(row, col));
    
    // Highlight the target byte
    QTableWidgetItem* target_item = item(row, col);
    if (target_item) {
        target_item->setBackground(QColor(255, 255, 0, 150)); // Light yellow highlight
    }
}

void MemoryView::on_item_changed(QTableWidgetItem* item) {
    // Handle memory editing
    if (!item || item->column() == 0 || item->column() == 17) {
        return; // Don't edit address or ASCII columns
    }
    
    int row = item->row();
    int col = item->column();
    int data_index = row * bytes_per_row + (col - 1);
    
    if (data_index >= static_cast<int>(memory_data.size())) {
        return;
    }
    
    // Validate hex input
    QString text = item->text().trimmed();
    bool ok;
    uint8_t new_value = text.toUInt(&ok, 16);
    
    if (!ok || text.length() > 2) {
        // Invalid input - revert
        uint8_t old_value = memory_data[data_index];
        item->setText(QString("%1").arg(old_value, 2, 16, QChar('0')).toUpper());
        QMessageBox::warning(this, "Invalid Input", "Please enter a valid hex byte (00-FF)");
        return;
    }
    
    // Update memory data
    memory_data[data_index] = new_value;
    
    // Update ASCII column
    update_ascii_for_row(row);
    
    // Emit signal for actual memory writing
    uint64_t address = base_address + data_index;
    emit memory_write_requested(address, new_value);
    
    // Mark as modified
    item->setBackground(QColor(255, 200, 200, 100)); // Light red for modified
}

void MemoryView::update_ascii_for_row(int row) {
    QString ascii_str;
    
    for (int col = 0; col < bytes_per_row; ++col) {
        int data_index = row * bytes_per_row + col;
        
        if (data_index < static_cast<int>(memory_data.size())) {
            uint8_t byte_value = memory_data[data_index];
            
            if (std::isprint(byte_value) && byte_value >= 32) {
                ascii_str += static_cast<char>(byte_value);
            } else {
                ascii_str += '.';
            }
        } else {
            ascii_str += ' ';
        }
    }
    
    QTableWidgetItem* ascii_item = item(row, 17);
    if (ascii_item) {
        ascii_item->setText(ascii_str);
    }
}

void MemoryView::contextMenuEvent(QContextMenuEvent* event) {
    QTableWidgetItem* clicked_item = itemAt(event->pos());
    if (!clicked_item) {
        return;
    }
    
    int row = clicked_item->row();
    int col = clicked_item->column();
    
    QMenu menu(this);
    
    // Get address for this position
    uint64_t row_address = base_address + (row * bytes_per_row);
    
    if (col == 0) {
        // Address column context menu
        QAction* copy_address = menu.addAction(QString("Copy Address (0x%1)")
                                             .arg(row_address, 16, 16, QChar('0')).toUpper());
        connect(copy_address, &QAction::triggered, [row_address] {
            QApplication::clipboard()->setText(QString("0x%1").arg(row_address, 16, 16, QChar('0')).toUpper());
        });
        
        QAction* goto_address = menu.addAction("Go to Address...");
        connect(goto_address, &QAction::triggered, this, &MemoryView::show_go_to_dialog);
        
    } else if (col >= 1 && col <= bytes_per_row) {
        // Hex byte column context menu
        uint64_t byte_address = row_address + (col - 1);
        uint8_t byte_value = 0;
        
        int data_index = row * bytes_per_row + (col - 1);
        if (data_index < static_cast<int>(memory_data.size())) {
            byte_value = memory_data[data_index];
        }
        
        QAction* copy_byte = menu.addAction(QString("Copy Byte (0x%1)")
                                          .arg(byte_value, 2, 16, QChar('0')).toUpper());
        connect(copy_byte, &QAction::triggered, [byte_value] {
            QApplication::clipboard()->setText(QString("0x%1").arg(byte_value, 2, 16, QChar('0')).toUpper());
        });
        
        QAction* copy_address = menu.addAction(QString("Copy Address (0x%1)")
                                             .arg(byte_address, 16, 16, QChar('0')).toUpper());
        connect(copy_address, &QAction::triggered, [byte_address] {
            QApplication::clipboard()->setText(QString("0x%1").arg(byte_address, 16, 16, QChar('0')).toUpper());
        });
        
        menu.addSeparator();
        
        QAction* edit_byte = menu.addAction("Edit Byte...");
        connect(edit_byte, &QAction::triggered, [this, clicked_item] {
            edit(indexFromItem(clicked_item));
        });
        
        QAction* set_breakpoint = menu.addAction("Set Breakpoint on Access");
        connect(set_breakpoint, &QAction::triggered, [this, byte_address] {
            emit breakpoint_requested(byte_address);
        });
        
    } else if (col == 17) {
        // ASCII column context menu
        QAction* copy_ascii = menu.addAction("Copy ASCII");
        connect(copy_ascii, &QAction::triggered, [clicked_item] {
            QApplication::clipboard()->setText(clicked_item->text());
        });
    }
    
    menu.addSeparator();
    
    // General actions
    QAction* refresh = menu.addAction("Refresh");
    connect(refresh, &QAction::triggered, [this] {
        refresh_memory(base_address, memory_data.size());
    });
    
    QAction* export_action = menu.addAction("Export Memory...");
    export_action->setEnabled(false); // TODO: Implement memory export
    
    menu.exec(event->globalPos());
}

void MemoryView::show_go_to_dialog() {
    bool ok;
    QString address_text = QInputDialog::getText(this, "Go to Address", 
                                               "Enter memory address (hex):", 
                                               QLineEdit::Normal, "", &ok);
    
    if (ok && !address_text.isEmpty()) {
        // Parse address
        QString clean_address = address_text;
        if (clean_address.startsWith("0x", Qt::CaseInsensitive)) {
            clean_address.remove(0, 2);
        }
        
        uint64_t address = clean_address.toULongLong(&ok, 16);
        if (ok) {
            navigate_to_address(address);
        } else {
            QMessageBox::warning(this, "Invalid Address", "Please enter a valid hex address.");
        }
    }
}

std::vector<uint8_t> MemoryView::get_selected_bytes() {
    std::vector<uint8_t> selected_bytes;
    
    QList<QTableWidgetItem*> selected_items = selectedItems();
    for (QTableWidgetItem* item : selected_items) {
        int col = item->column();
        if (col >= 1 && col <= bytes_per_row) {
            int row = item->row();
            int data_index = row * bytes_per_row + (col - 1);
            
            if (data_index < static_cast<int>(memory_data.size())) {
                selected_bytes.push_back(memory_data[data_index]);
            }
        }
    }
    
    return selected_bytes;
}

void MemoryView::highlight_address_range(uint64_t start_addr, uint64_t end_addr) {
    // Clear previous highlights
    for (int row = 0; row < rowCount(); ++row) {
        for (int col = 1; col <= bytes_per_row; ++col) {
            QTableWidgetItem* item_ptr = item(row, col);
            if (item_ptr) {
                item_ptr->setBackground(QBrush());
            }
        }
    }
    
    // Apply new highlights
    for (uint64_t addr = start_addr; addr <= end_addr; ++addr) {
        if (addr < base_address || addr >= base_address + memory_data.size()) {
            continue;
        }
        
        uint64_t offset = addr - base_address;
        int row = offset / bytes_per_row;
        int col = (offset % bytes_per_row) + 1;
        
        QTableWidgetItem* item_ptr = item(row, col);
        if (item_ptr) {
            item_ptr->setBackground(QColor(255, 255, 0, 100)); // Light yellow
        }
    }
}

} // namespace debugger 