#include "main_window.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/QMenu>
#include <QtWidgets/QAction>
#include <QtWidgets/QTextCursor>
#include <QtWidgets/QScrollBar>
#include <QtCore/QRegularExpression>
#include <QtGui/QTextCharFormat>
#include <QtGui/QPainter>
#include <QtGui/QFontMetrics>
#include <QtGui/QMouseEvent>
#include <QtGui/QContextMenuEvent>

namespace debugger {

// Disassembly view implementation (basic stubs)
// The implementation is already included in main_window.cpp

DisassemblyView::DisassemblyView(QWidget* parent) 
    : QTextEdit(parent), highlighted_address(0), current_line(-1) {
    
    // Set monospace font for consistent formatting
    QFont font("Consolas", 10);
    font.setStyleHint(QFont::Monospace);
    setFont(font);
    
    // Configure text edit properties
    setReadOnly(true);
    setLineWrapMode(QTextEdit::NoWrap);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    
    // Enable syntax highlighting
    setup_syntax_highlighting();
    
    // Set up line number area
    line_number_area = new LineNumberArea(this);
    
    // Connect signals for line number updates
    connect(this, &QTextEdit::textChanged, this, &DisassemblyView::update_line_number_area_width);
    connect(verticalScrollBar(), &QScrollBar::valueChanged, line_number_area, &LineNumberArea::update);
    connect(this, &QTextEdit::cursorPositionChanged, this, &DisassemblyView::highlight_current_line);
    
    update_line_number_area_width();
    highlight_current_line();
}

void DisassemblyView::set_instructions(const std::vector<Instruction>& instructions) {
    current_instructions = instructions;
    
    // Build instruction lookup map for fast address-to-line mapping
    address_to_line.clear();
    line_to_address.clear();
    
    QString content;
    content.reserve(instructions.size() * 80); // Estimate average line length
    
    // Header
    content += QString("Disassembly View - %1 instructions loaded\n")
               .arg(instructions.size());
    content += QString("=====================================\n\n");
    
    int line_number = 3; // Start after header
    
    for (size_t i = 0; i < instructions.size(); ++i) {
        const auto& insn = instructions[i];
        
        // Map address to line number
        address_to_line[insn.address] = line_number;
        line_to_address[line_number] = insn.address;
        
        // Format instruction bytes
        QString bytes_str;
        for (size_t j = 0; j < insn.bytes.size() && j < 8; ++j) {
            bytes_str += QString("%1 ").arg(insn.bytes[j], 2, 16, QChar('0')).toUpper();
        }
        bytes_str = bytes_str.leftJustified(24); // Pad to consistent width
        
        // Format the instruction line
        QString line = QString("%1:  %2 %3 %4")
                      .arg(insn.address, 16, 16, QChar('0'))
                      .arg(bytes_str)
                      .arg(QString::fromStdString(insn.mnemonic).leftJustified(8))
                      .arg(QString::fromStdString(insn.operands));
        
        // Add instruction type annotations
        if (insn.is_call) {
            line += QString("    ; CALL -> 0x%1").arg(insn.target_address, 0, 16);
        } else if (insn.is_jump) {
            line += QString("    ; JMP -> 0x%1").arg(insn.target_address, 0, 16);
        } else if (insn.is_return) {
            line += "    ; RETURN";
        }
        
        content += line + "\n";
        line_number++;
    }
    
    setPlainText(content);
    
    // Apply syntax highlighting
    apply_syntax_highlighting();
    
    // Update line number area
    update_line_number_area_width();
}

void DisassemblyView::highlight_instruction(uint64_t address) {
    highlighted_address = address;
    
    auto it = address_to_line.find(address);
    if (it != address_to_line.end()) {
        int line_num = it->second;
        
        // Move cursor to the line
        QTextCursor cursor = textCursor();
        cursor.movePosition(QTextCursor::Start);
        cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, line_num - 1);
        setTextCursor(cursor);
        
        // Ensure the line is visible
        ensureCursorVisible();
        
        // Highlight the line
        current_line = line_num;
        highlight_current_line();
    }
}

void DisassemblyView::clear_highlight() {
    highlighted_address = 0;
    current_line = -1;
    
    // Clear extra selections (highlighting)
    QList<QTextEdit::ExtraSelection> selections;
    setExtraSelections(selections);
}

void DisassemblyView::mouseDoubleClickEvent(QMouseEvent* event) {
    QTextEdit::mouseDoubleClickEvent(event);
    
    // Get the line that was double-clicked
    QTextCursor cursor = cursorForPosition(event->pos());
    int line_number = cursor.blockNumber() + 1;
    
    // Find address for this line
    auto it = line_to_address.find(line_number);
    if (it != line_to_address.end()) {
        uint64_t address = it->second;
        
        // Emit signal for navigation (you can connect this in MainWindow)
        emit address_double_clicked(address);
    }
}

void DisassemblyView::contextMenuEvent(QContextMenuEvent* event) {
    QMenu* menu = createStandardContextMenu();
    
    // Get the line under cursor
    QTextCursor cursor = cursorForPosition(event->pos());
    int line_number = cursor.blockNumber() + 1;
    
    auto it = line_to_address.find(line_number);
    if (it != line_to_address.end()) {
        uint64_t address = it->second;
        
        menu->addSeparator();
        
        // Add breakpoint action
        QAction* breakpoint_action = menu->addAction("Toggle Breakpoint");
        connect(breakpoint_action, &QAction::triggered, [this, address] {
            emit toggle_breakpoint_requested(address);
        });
        
        // Add navigation actions
        QAction* goto_action = menu->addAction("Go to Address...");
        connect(goto_action, &QAction::triggered, [this] {
            emit go_to_address_requested();
        });
        
        QAction* copy_address = menu->addAction(QString("Copy Address (0x%1)").arg(address, 0, 16));
        connect(copy_address, &QAction::triggered, [address] {
            QApplication::clipboard()->setText(QString("0x%1").arg(address, 0, 16));
        });
        
        // Add analysis actions for calls/jumps
        for (const auto& insn : current_instructions) {
            if (insn.address == address) {
                if (insn.is_call || insn.is_jump) {
                    QAction* follow_action = menu->addAction(
                        QString("Follow %1 (0x%2)")
                        .arg(insn.is_call ? "Call" : "Jump")
                        .arg(insn.target_address, 0, 16)
                    );
                    connect(follow_action, &QAction::triggered, [this, insn] {
                        emit address_double_clicked(insn.target_address);
                    });
                }
                break;
            }
        }
    }
    
    menu->exec(event->globalPos());
    delete menu;
}

void DisassemblyView::setup_syntax_highlighting() {
    // Create format for different instruction types
    mnemonic_format.setForeground(QColor(86, 156, 214));  // Blue
    mnemonic_format.setFontWeight(QFont::Bold);
    
    operand_format.setForeground(QColor(220, 220, 170));   // Light yellow
    
    address_format.setForeground(QColor(181, 206, 168));   // Light green
    
    comment_format.setForeground(QColor(106, 153, 85));    // Green
    comment_format.setFontItalic(true);
    
    bytes_format.setForeground(QColor(156, 220, 254));     // Light blue
    
    call_format.setForeground(QColor(255, 128, 128));      // Light red
    call_format.setFontWeight(QFont::Bold);
    
    jump_format.setForeground(QColor(255, 165, 0));        // Orange
    jump_format.setFontWeight(QFont::Bold);
}

void DisassemblyView::apply_syntax_highlighting() {
    QTextDocument* doc = document();
    QTextCursor cursor(doc);
    
    // Regular expressions for different parts
    QRegularExpression address_regex("^([0-9A-Fa-f]{16}):");
    QRegularExpression bytes_regex(":\\s+([0-9A-Fa-f\\s]+)\\s+");
    QRegularExpression mnemonic_regex("\\s+([a-zA-Z][a-zA-Z0-9]*)\\s+");
    QRegularExpression comment_regex(";.*$");
    QRegularExpression call_regex("\\b(call|CALL)\\b");
    QRegularExpression jump_regex("\\b(j[a-zA-Z]+|JMP|JZ|JNZ|JE|JNE)\\b");
    
    cursor.movePosition(QTextCursor::Start);
    
    while (!cursor.atEnd()) {
        cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
        QString line_text = cursor.selectedText();
        
        // Reset formatting for this line
        QTextCharFormat default_format;
        cursor.setCharFormat(default_format);
        
        // Apply highlighting based on patterns
        apply_format_to_matches(cursor, line_text, address_regex, address_format);
        apply_format_to_matches(cursor, line_text, bytes_regex, bytes_format);
        apply_format_to_matches(cursor, line_text, call_regex, call_format);
        apply_format_to_matches(cursor, line_text, jump_regex, jump_format);
        apply_format_to_matches(cursor, line_text, comment_regex, comment_format);
        
        cursor.movePosition(QTextCursor::NextBlock);
    }
}

void DisassemblyView::apply_format_to_matches(QTextCursor& cursor, const QString& text, 
                                            const QRegularExpression& regex, 
                                            const QTextCharFormat& format) {
    QRegularExpressionMatchIterator matches = regex.globalMatch(text);
    
    while (matches.hasNext()) {
        QRegularExpressionMatch match = matches.next();
        
        QTextCursor format_cursor = cursor;
        format_cursor.movePosition(QTextCursor::StartOfLine);
        format_cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, match.capturedStart());
        format_cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, match.capturedLength());
        
        format_cursor.setCharFormat(format);
    }
}

void DisassemblyView::highlight_current_line() {
    QList<QTextEdit::ExtraSelection> selections;
    
    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;
        
        QColor line_color = QColor(Qt::yellow).lighter(160);
        selection.format.setBackground(line_color);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        
        selections.append(selection);
    }
    
    // Highlight the instruction at highlighted_address
    if (highlighted_address != 0) {
        auto it = address_to_line.find(highlighted_address);
        if (it != address_to_line.end()) {
            QTextEdit::ExtraSelection highlight_selection;
            
            QColor highlight_color = QColor(Qt::cyan).lighter(160);
            highlight_selection.format.setBackground(highlight_color);
            highlight_selection.format.setProperty(QTextFormat::FullWidthSelection, true);
            
            // Position cursor at the highlighted line
            QTextCursor highlight_cursor = textCursor();
            highlight_cursor.movePosition(QTextCursor::Start);
            highlight_cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, it->second - 1);
            highlight_selection.cursor = highlight_cursor;
            
            selections.append(highlight_selection);
        }
    }
    
    setExtraSelections(selections);
}

void DisassemblyView::update_line_number_area_width() {
    setViewportMargins(line_number_area_width(), 0, 0, 0);
}

int DisassemblyView::line_number_area_width() {
    int digits = 1;
    int max_line = qMax(1, blockCount());
    while (max_line >= 10) {
        max_line /= 10;
        ++digits;
    }
    
    int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
    return space;
}

void DisassemblyView::line_number_area_paint_event(QPaintEvent* event) {
    QPainter painter(line_number_area);
    painter.fillRect(event->rect(), QColor(240, 240, 240));
    
    QTextBlock block = firstVisibleBlock();
    int block_number = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());
    
    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(block_number + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, line_number_area->width(), 
                           fontMetrics().height(), Qt::AlignRight, number);
        }
        
        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++block_number;
    }
}

void DisassemblyView::resizeEvent(QResizeEvent* event) {
    QTextEdit::resizeEvent(event);
    
    QRect cr = contentsRect();
    line_number_area->setGeometry(QRect(cr.left(), cr.top(), 
                                      line_number_area_width(), cr.height()));
}

// LineNumberArea implementation
LineNumberArea::LineNumberArea(DisassemblyView* editor) : QWidget(editor), code_editor(editor) {}

QSize LineNumberArea::sizeHint() const {
    return QSize(code_editor->line_number_area_width(), 0);
}

void LineNumberArea::paintEvent(QPaintEvent* event) {
    code_editor->line_number_area_paint_event(event);
}

} // namespace debugger 