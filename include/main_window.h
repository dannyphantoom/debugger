#pragma once

#include "disassembler.h"
#include "decompiler.h"
#include "debugger_engine.h"
#include "elf_parser.h"

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtCore/QTimer>
#include <memory>

namespace debugger {

class DisassemblyView;
class DecompilerView;
class MemoryView;
class RegistersView;
class BreakpointView;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    virtual ~MainWindow();

    // File operations
    bool open_file(const QString& filename);
    void close_file();

public slots:
    // Menu actions
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
    
    // View actions
    void on_action_go_to_address_triggered();
    void on_action_find_triggered();
    void on_action_toggle_breakpoint_triggered();
    
    // Analysis actions
    void on_action_analyze_functions_triggered();
    void on_action_show_strings_triggered();
    void on_action_show_imports_triggered();
    void on_action_show_exports_triggered();

private slots:
    void update_debug_state();
    void on_breakpoint_hit(uint64_t address);
    void on_function_selected(uint64_t address);
    void on_address_double_clicked(uint64_t address);
    void refresh_views();

private:
    // Core components
    std::unique_ptr<Disassembler> disassembler;
    std::unique_ptr<Decompiler> decompiler;
    std::unique_ptr<DebuggerEngine> debugger_engine;
    std::unique_ptr<ElfParser> elf_parser;

    // GUI components
    QSplitter* main_splitter;
    QSplitter* left_splitter;
    QSplitter* right_splitter;
    QTabWidget* left_tabs;
    QTabWidget* center_tabs;
    QTabWidget* right_tabs;

    // Views
    DisassemblyView* disassembly_view;
    DecompilerView* decompiler_view;
    MemoryView* memory_view;
    RegistersView* registers_view;
    BreakpointView* breakpoint_view;
    
    // Information panels
    QTreeWidget* functions_tree;
    QTreeWidget* symbols_tree;
    QTableWidget* sections_table;
    QTableWidget* imports_table;
    QTableWidget* exports_table;
    QTextEdit* strings_view;
    QTextEdit* log_view;

    // Status and control
    QLabel* status_label;
    QLabel* architecture_label;
    QLabel* debug_state_label;
    QPushButton* continue_button;
    QPushButton* pause_button;
    QPushButton* step_into_button;
    QPushButton* step_over_button;
    QPushButton* step_out_button;

    // Current state
    QString current_filename;
    Architecture current_architecture;
    DebuggerState current_debug_state;
    uint64_t current_address;
    QTimer* refresh_timer;

    // Helper functions
    void setup_ui();
    void setup_menus();
    void setup_toolbars();
    void setup_status_bar();
    void connect_signals();
    void update_title();
    void update_status();
    void populate_functions_tree();
    void populate_symbols_tree();
    void populate_sections_table();
    void populate_imports_table();
    void populate_exports_table();
    void populate_strings_view();
    void log_message(const QString& message);
    void show_error(const QString& message);
    void show_info(const QString& message);
    bool confirm_action(const QString& message);
    void navigate_to_address(uint64_t address);
    void highlight_current_instruction(uint64_t address);
    void update_debug_controls();
    void save_settings();
    void load_settings();
};

// Simplified view classes without Q_OBJECT
class DisassemblyView : public QTextEdit {
public:
    explicit DisassemblyView(QWidget* parent = nullptr);
    virtual ~DisassemblyView() = default;
    void set_instructions(const std::vector<Instruction>& instructions);
    void highlight_instruction(uint64_t address);
    void clear_highlight();

protected:
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;

private:
    std::vector<Instruction> current_instructions;
    uint64_t highlighted_address;
};

class DecompilerView : public QTextEdit {
public:
    explicit DecompilerView(QWidget* parent = nullptr);
    virtual ~DecompilerView() = default;
    void set_decompiled_code(const std::string& code);
    void clear_code();

private:
    void setup_syntax_highlighting();
};

class MemoryView : public QTableWidget {
public:
    explicit MemoryView(QWidget* parent = nullptr);
    virtual ~MemoryView() = default;
    void set_memory_data(uint64_t start_address, const std::vector<uint8_t>& data);
    void refresh_memory(uint64_t address, size_t size = 256);

private:
    uint64_t base_address;
    std::vector<uint8_t> memory_data;
    void update_display();
};

class RegistersView : public QTableWidget {
public:
    explicit RegistersView(QWidget* parent = nullptr);
    virtual ~RegistersView() = default;
    void set_registers(const std::vector<Register>& registers);
    void highlight_modified_registers();

private:
    std::vector<Register> current_registers;
    void setup_columns();
};

class BreakpointView : public QTableWidget {
public:
    explicit BreakpointView(QWidget* parent = nullptr);
    virtual ~BreakpointView() = default;
    void set_breakpoints(const std::vector<Breakpoint>& breakpoints);
    void add_breakpoint(const Breakpoint& bp);
    void remove_breakpoint(uint64_t address);

private:
    void setup_columns();
    void update_breakpoint_list();
};

} // namespace debugger 