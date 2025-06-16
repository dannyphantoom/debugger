#pragma once

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/QHeaderView>
#include <QtGui/QTextCharFormat>
#include <QtCore/QRegularExpression>
#include <memory>
#include <map>

#include "disassembler.h"
#include "decompiler.h"
#include "debugger_engine.h"
#include "elf_parser.h"

namespace debugger {

// Forward declaration
class LineNumberArea;

class DisassemblyView : public QTextEdit {
    Q_OBJECT
public:
    explicit DisassemblyView(QWidget* parent = nullptr);
    
    void set_instructions(const std::vector<Instruction>& instructions);
    void highlight_instruction(uint64_t address);
    void clear_highlight();
    void clear() { QTextEdit::clear(); }
    
    // Line number area support
    void line_number_area_paint_event(QPaintEvent* event);
    int line_number_area_width();

signals:
    void address_double_clicked(uint64_t address);
    void toggle_breakpoint_requested(uint64_t address);
    void go_to_address_requested();

protected:
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void update_line_number_area_width();
    void highlight_current_line();

private:
    void setup_syntax_highlighting();
    void apply_syntax_highlighting();
    void apply_format_to_matches(QTextCursor& cursor, const QString& text, 
                               const QRegularExpression& regex, 
                               const QTextCharFormat& format);
    
    std::vector<Instruction> current_instructions;
    uint64_t highlighted_address;
    int current_line;
    
    // Address-to-line mapping for fast navigation
    std::map<uint64_t, int> address_to_line;
    std::map<int, uint64_t> line_to_address;
    
    // Line number area
    LineNumberArea* line_number_area;
    
    // Syntax highlighting formats
    QTextCharFormat mnemonic_format;
    QTextCharFormat operand_format;
    QTextCharFormat address_format;
    QTextCharFormat comment_format;
    QTextCharFormat bytes_format;
    QTextCharFormat call_format;
    QTextCharFormat jump_format;
};

class LineNumberArea : public QWidget {
public:
    LineNumberArea(DisassemblyView* editor);
    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    DisassemblyView* code_editor;
};

// Forward declaration for syntax highlighter
class CppSyntaxHighlighter;

class DecompilerView : public QTextEdit {
    Q_OBJECT
public:
    explicit DecompilerView(QWidget* parent = nullptr);
    
    void set_decompiled_code(const std::string& code);
    void clear_code();
    void setup_syntax_highlighting();
    void append_decompiled_function(const std::string& function_code);
    void navigate_to_function(const std::string& function_name);
    void highlight_line(int line_number);
    void add_analysis_comment(const std::string& comment);
    QString get_selected_text();
    void find_text(const QString& text);

signals:
    void function_analysis_requested();
    void goto_assembly_requested();

protected:
    void contextMenuEvent(QContextMenuEvent* event) override;

private:
    CppSyntaxHighlighter* syntax_highlighter;
};

class MemoryView : public QTableWidget {
    Q_OBJECT
public:
    explicit MemoryView(QWidget* parent = nullptr);
    
    void set_memory_data(uint64_t address, const std::vector<uint8_t>& data);
    void refresh_memory(uint64_t address, size_t size);
    void update_display();
    void navigate_to_address(uint64_t address);
    void highlight_address_range(uint64_t start_addr, uint64_t end_addr);
    std::vector<uint8_t> get_selected_bytes();

signals:
    void memory_refresh_requested(uint64_t address, size_t size);
    void memory_write_requested(uint64_t address, uint8_t value);
    void breakpoint_requested(uint64_t address);

protected:
    void contextMenuEvent(QContextMenuEvent* event) override;

private slots:
    void on_item_changed(QTableWidgetItem* item);
    void show_go_to_dialog();

private:
    void setup_table();
    void update_ascii_for_row(int row);
    
    uint64_t base_address;
    std::vector<uint8_t> memory_data;
    int bytes_per_row;
    QFont mono_font;
};

class RegistersView : public QTableWidget {
    Q_OBJECT
public:
    explicit RegistersView(QWidget* parent = nullptr);
    
    void set_registers(const std::vector<Register>& registers);
    void highlight_modified_registers();
    void setup_columns();
    void clear_highlights();

signals:
    void navigate_to_address_requested(uint64_t address);
    void view_memory_requested(uint64_t address);

protected:
    void contextMenuEvent(QContextMenuEvent* event) override;

private:
    void setup_table();
    void initialize_default_registers();
    QString format_flags_register(uint64_t flags);
    
    std::vector<Register> previous_registers;
};

class BreakpointView : public QTableWidget {
    Q_OBJECT
public:
    explicit BreakpointView(QWidget* parent = nullptr);
    
    void set_breakpoints(const std::vector<Breakpoint>& breakpoints);
    void add_breakpoint(const Breakpoint& bp);
    void remove_breakpoint(uint64_t address);
    void setup_columns();
    void update_breakpoint_list();

private:
    std::vector<Breakpoint> current_breakpoints;
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    bool open_file(const QString& filename);
    void close_file();
    void update_file_analysis();

private slots:
    void on_action_open_triggered();
    void on_action_close_triggered();
    void on_action_exit_triggered();
    void on_action_about_triggered();
    
    // Debug actions
    void on_action_start_debug_triggered();
    void on_action_attach_process_triggered();
    void on_action_continue_triggered();
    void on_action_pause_triggered();
    void on_action_stop_triggered();
    void on_action_step_into_triggered();
    void on_action_step_over_triggered();
    void on_action_step_out_triggered();
    
    // Navigation actions
    void on_action_go_to_address_triggered();
    void on_action_find_triggered();
    void on_action_toggle_breakpoint_triggered();
    
    // Analysis actions
    void on_action_analyze_functions_triggered();
    void on_action_show_strings_triggered();
    void on_action_show_imports_triggered();
    void on_action_show_exports_triggered();
    
    // Internal slots
    void update_debug_state();
    void on_breakpoint_hit(uint64_t address);
    void on_function_selected(uint64_t address);
    void on_address_double_clicked(uint64_t address);
    void refresh_views();

private:
    void setup_ui();
    void setup_left_panel(QSplitter* parent);
    void setup_center_panel(QSplitter* parent);
    void setup_right_panel(QSplitter* parent);
    void setup_menus();
    void setup_toolbars();
    void setup_status_bar();
    void apply_dark_theme();
    void connect_signals();
    
    void load_settings();
    void save_settings();
    
    void update_status_bar();
    void update_title();
    void update_status();
    void log_message(const QString& message);
    void show_error(const QString& message);
    void show_info(const QString& message);
    bool confirm_action(const QString& message);
    void navigate_to_address(uint64_t address);
    void highlight_current_instruction(uint64_t address);
    void update_debug_controls();
    
    void populate_functions_tree();
    void populate_symbols_tree();
    void populate_sections_table();
    void populate_strings_view();
    void update_strings_view(const std::vector<std::string>& strings);
    
    // Core components
    std::unique_ptr<Disassembler> disassembler;
    std::unique_ptr<Decompiler> decompiler;
    std::unique_ptr<DebuggerEngine> debugger_engine;
    std::unique_ptr<ElfParser> elf_parser;
    
    // UI Components
    QTabWidget* left_tabs;
    QTabWidget* center_tabs;
    QTabWidget* right_tabs;
    QSplitter* main_splitter;
    
    // Left panel
    QTreeWidget* functions_tree;
    QTreeWidget* symbols_tree;
    QTableWidget* sections_table;
    QTextEdit* strings_view;
    
    // Center panel
    DisassemblyView* disassembly_view;
    DecompilerView* decompiler_view;
    
    // Right panel
    RegistersView* registers_view;
    MemoryView* memory_view;
    BreakpointView* breakpoint_view;
    QTextEdit* log_view;
    
    // Debug controls
    QPushButton* continue_button;
    QPushButton* pause_button;
    QPushButton* step_into_button;
    QPushButton* step_over_button;
    QPushButton* step_out_button;
    
    // Status bar
    QLabel* status_label;
    QLabel* architecture_label;
    QLabel* debug_state_label;
    
    // State
    QString current_filename;
    Architecture current_architecture;
    DebuggerState current_debug_state;
    uint64_t current_address;
};

} // namespace debugger 