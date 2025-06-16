#include "main_window.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QLabel>
#include <QtCore/QDir>
#include <QtCore/QStandardPaths>
#include <QtCore/QSettings>
#include <QtCore/QDateTime>
#include <QtCore/QDebug>

namespace debugger {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , current_architecture(Architecture::UNKNOWN)
    , current_debug_state(DebuggerState::NOT_RUNNING)
    , current_address(0)
{
    // Initialize core components
    disassembler = std::make_unique<Disassembler>();
    decompiler = std::make_unique<Decompiler>();
    debugger_engine = std::make_unique<DebuggerEngine>();
    elf_parser = std::make_unique<ElfParser>();
    
    // Setup UI
    setup_ui();
    setup_menus();
    setup_toolbars();
    setup_status_bar();
    connect_signals();
    
    // Initialize refresh timer
    refresh_timer = new QTimer(this);
    refresh_timer->setSingleShot(false);
    refresh_timer->setInterval(100); // 100ms refresh rate
    connect(refresh_timer, &QTimer::timeout, this, &MainWindow::refresh_views);
    
    // Load settings
    load_settings();
    
    // Set window properties
    setWindowTitle("Advanced Debugger v1.0.0");
    setMinimumSize(800, 600);
    resize(1200, 800);
}

MainWindow::~MainWindow() {
    save_settings();
}

void MainWindow::setup_ui() {
    // Create central widget and main splitter
    QWidget* central_widget = new QWidget(this);
    setCentralWidget(central_widget);
    
    QVBoxLayout* main_layout = new QVBoxLayout(central_widget);
    main_layout->setContentsMargins(5, 5, 5, 5);
    
    // Create simple text edit for now
    QTextEdit* text_edit = new QTextEdit(this);
    text_edit->setPlainText("Advanced Debugger v1.0.0\n\nLoad an ELF file to begin analysis.\n\nFeatures:\n- Multi-architecture disassembly\n- Basic decompilation\n- Debug capabilities\n\nUse File -> Open to load a binary file.");
    text_edit->setReadOnly(true);
    main_layout->addWidget(text_edit);
    
    // Create debug control toolbar
    QHBoxLayout* control_layout = new QHBoxLayout();
    continue_button = new QPushButton("Continue (F5)", this);
    pause_button = new QPushButton("Pause", this);
    step_into_button = new QPushButton("Step Into (F11)", this);
    step_over_button = new QPushButton("Step Over (F10)", this);
    step_out_button = new QPushButton("Step Out (Shift+F11)", this);
    
    control_layout->addWidget(continue_button);
    control_layout->addWidget(pause_button);
    control_layout->addWidget(step_into_button);
    control_layout->addWidget(step_over_button);
    control_layout->addWidget(step_out_button);
    control_layout->addStretch();
    
    main_layout->addLayout(control_layout);
    
    // Create main horizontal splitter
    main_splitter = new QSplitter(Qt::Horizontal, this);
    main_layout->addWidget(main_splitter);
    
    // Left panel - Analysis trees
    left_tabs = new QTabWidget(this);
    left_tabs->setMaximumWidth(300);
    
    functions_tree = new QTreeWidget(this);
    functions_tree->setHeaderLabel("Functions");
    left_tabs->addTab(functions_tree, "Functions");
    
    symbols_tree = new QTreeWidget(this);
    symbols_tree->setHeaderLabel("Symbols");
    left_tabs->addTab(symbols_tree, "Symbols");
    
    sections_table = new QTableWidget(this);
    sections_table->setColumnCount(4);
    sections_table->setHorizontalHeaderLabels({"Name", "Address", "Size", "Type"});
    left_tabs->addTab(sections_table, "Sections");
    
    imports_table = new QTableWidget(this);
    imports_table->setColumnCount(3);
    imports_table->setHorizontalHeaderLabels({"Name", "Address", "Type"});
    left_tabs->addTab(imports_table, "Imports");
    
    exports_table = new QTableWidget(this);
    exports_table->setColumnCount(3);
    exports_table->setHorizontalHeaderLabels({"Name", "Address", "Type"});
    left_tabs->addTab(exports_table, "Exports");
    
    strings_view = new QTextEdit(this);
    strings_view->setReadOnly(true);
    left_tabs->addTab(strings_view, "Strings");
    
    main_splitter->addWidget(left_tabs);
    
    // Center panel - Disassembly and decompiler
    center_tabs = new QTabWidget(this);
    
    disassembly_view = new DisassemblyView(this);
    center_tabs->addTab(disassembly_view, "Disassembly");
    
    decompiler_view = new DecompilerView(this);
    center_tabs->addTab(decompiler_view, "Decompiler");
    
    main_splitter->addWidget(center_tabs);
    
    // Right panel - Debug info
    right_tabs = new QTabWidget(this);
    right_tabs->setMaximumWidth(400);
    
    registers_view = new RegistersView(this);
    right_tabs->addTab(registers_view, "Registers");
    
    memory_view = new MemoryView(this);
    right_tabs->addTab(memory_view, "Memory");
    
    breakpoint_view = new BreakpointView(this);
    right_tabs->addTab(breakpoint_view, "Breakpoints");
    
    log_view = new QTextEdit(this);
    log_view->setReadOnly(true);
    right_tabs->addTab(log_view, "Log");
    
    main_splitter->addWidget(right_tabs);
    
    // Set splitter proportions
    main_splitter->setSizes({250, 800, 350});
    
    update_debug_controls();
}

void MainWindow::setup_menus() {
    QMenuBar* menu_bar = menuBar();
    
    // File menu
    QMenu* file_menu = menu_bar->addMenu("&File");
    
    QAction* open_action = file_menu->addAction("&Open Binary...");
    open_action->setShortcut(QKeySequence::Open);
    connect(open_action, &QAction::triggered, this, &MainWindow::on_action_open_triggered);
    
    QAction* close_action = file_menu->addAction("&Close");
    close_action->setShortcut(QKeySequence::Close);
    connect(close_action, &QAction::triggered, this, &MainWindow::on_action_close_triggered);
    
    file_menu->addSeparator();
    
    QAction* exit_action = file_menu->addAction("E&xit");
    exit_action->setShortcut(QKeySequence::Quit);
    connect(exit_action, &QAction::triggered, this, &MainWindow::on_action_exit_triggered);
    
    // Debug menu
    QMenu* debug_menu = menu_bar->addMenu("&Debug");
    
    QAction* start_action = debug_menu->addAction("&Start Debugging");
    connect(start_action, &QAction::triggered, this, &MainWindow::on_action_start_debug_triggered);
    
    // Help menu
    QMenu* help_menu = menu_bar->addMenu("&Help");
    QAction* about_action = help_menu->addAction("&About");
    connect(about_action, &QAction::triggered, this, &MainWindow::on_action_about_triggered);
}

void MainWindow::setup_toolbars() {
    QToolBar* main_toolbar = addToolBar("Main");
    
    main_toolbar->addAction("Open", this, &MainWindow::on_action_open_triggered);
    main_toolbar->addSeparator();
    main_toolbar->addAction("Start Debug", this, &MainWindow::on_action_start_debug_triggered);
    main_toolbar->addAction("Step Into", this, &MainWindow::on_action_step_into_triggered);
    main_toolbar->addAction("Step Over", this, &MainWindow::on_action_step_over_triggered);
}

void MainWindow::setup_status_bar() {
    status_label = new QLabel("Ready", this);
    architecture_label = new QLabel("", this);
    debug_state_label = new QLabel("Not Running", this);
    
    statusBar()->addWidget(status_label);
    statusBar()->addPermanentWidget(architecture_label);
    statusBar()->addPermanentWidget(debug_state_label);
}

void MainWindow::connect_signals() {
    // Connect debug control buttons
    connect(continue_button, &QPushButton::clicked, this, &MainWindow::on_action_continue_triggered);
    connect(pause_button, &QPushButton::clicked, this, &MainWindow::on_action_pause_triggered);
    connect(step_into_button, &QPushButton::clicked, this, &MainWindow::on_action_step_into_triggered);
    connect(step_over_button, &QPushButton::clicked, this, &MainWindow::on_action_step_over_triggered);
    connect(step_out_button, &QPushButton::clicked, this, &MainWindow::on_action_step_out_triggered);
    
    // Connect tree widget signals
    connect(functions_tree, &QTreeWidget::itemDoubleClicked, [this](QTreeWidgetItem* item) {
        bool ok;
        uint64_t address = item->text(1).toULongLong(&ok, 16);
        if (ok) {
            navigate_to_address(address);
        }
    });
}

bool MainWindow::open_file(const QString& filename) {
    if (filename.isEmpty()) {
        return false;
    }
    
    log_message(QString("Opening file: %1").arg(filename));
    
    // Try to load the ELF file
    if (!elf_parser->load_file(filename.toStdString())) {
        show_error(QString("Failed to load ELF file: %1").arg(QString::fromStdString(elf_parser->get_last_error())));
        return false;
    }
    
    current_filename = filename;
    current_architecture = elf_parser->detect_architecture();
    
    // Initialize disassembler with detected architecture
    if (!disassembler->initialize(current_architecture)) {
        show_error("Failed to initialize disassembler for detected architecture");
        return false;
    }
    
    // Initialize decompiler
    decompiler->set_architecture(current_architecture);
    
    // Update UI
    update_title();
    update_status();
    populate_sections_table();
    populate_functions_tree();
    populate_symbols_tree();
    populate_imports_table();
    populate_exports_table();
    populate_strings_view();
    
    // Disassemble and display code section
    std::vector<uint8_t> code_data = elf_parser->get_code_section_data();
    if (!code_data.empty()) {
        uint64_t entry_point = elf_parser->get_entry_point();
        std::vector<Instruction> instructions = disassembler->disassemble(
            code_data.data(), code_data.size(), entry_point);
        disassembly_view->set_instructions(instructions);
        
        log_message(QString("Disassembled %1 instructions").arg(instructions.size()));
    }
    
    log_message("File loaded successfully");
    return true;
}

void MainWindow::close_file() {
    current_filename.clear();
    current_architecture = Architecture::UNKNOWN;
    
    // Clear views
    disassembly_view->clear();
    decompiler_view->clear_code();
    functions_tree->clear();
    symbols_tree->clear();
    sections_table->setRowCount(0);
    imports_table->setRowCount(0);
    exports_table->setRowCount(0);
    strings_view->clear();
    
    update_title();
    update_status();
    
    log_message("File closed");
}

void MainWindow::on_action_open_triggered() {
    QString filename = QFileDialog::getOpenFileName(this,
        "Open Binary File", QDir::homePath(), "All Files (*)");
    
    if (!filename.isEmpty()) {
        open_file(filename);
    }
}

void MainWindow::on_action_close_triggered() {
    close_file();
}

void MainWindow::on_action_exit_triggered() {
    close();
}

void MainWindow::on_action_about_triggered() {
    QMessageBox::about(this, "About Advanced Debugger",
        "Advanced Debugger v1.0.0\n\n"
        "A comprehensive GUI-based debugger with disassembly, "
        "decompilation, and debugging capabilities.\n\n"
        "Built with Qt5, Capstone Engine, and ELFIO.");
}

void MainWindow::on_action_start_debug_triggered() {
    QMessageBox::information(this, "Debug", "Debug functionality not fully implemented yet");
}

void MainWindow::on_action_attach_process_triggered() {
    log_message("Attach to process requested (not implemented yet)");
}

void MainWindow::on_action_continue_triggered() {
    log_message("Continue execution requested (not implemented yet)");
}

void MainWindow::on_action_pause_triggered() {
    log_message("Pause execution requested (not implemented yet)");
}

void MainWindow::on_action_stop_triggered() {
    log_message("Stop execution requested (not implemented yet)");
}

void MainWindow::on_action_step_into_triggered() {
    log_message("Step into requested (not implemented yet)");
}

void MainWindow::on_action_step_over_triggered() {
    log_message("Step over requested (not implemented yet)");
}

void MainWindow::on_action_step_out_triggered() {
    log_message("Step out requested (not implemented yet)");
}

void MainWindow::on_action_go_to_address_triggered() {
    log_message("Go to address requested (not implemented yet)");
}

void MainWindow::on_action_find_triggered() {
    log_message("Find requested (not implemented yet)");
}

void MainWindow::on_action_toggle_breakpoint_triggered() {
    log_message("Toggle breakpoint requested (not implemented yet)");
}

void MainWindow::on_action_analyze_functions_triggered() {
    log_message("Function analysis requested (not implemented yet)");
}

void MainWindow::on_action_show_strings_triggered() {
    log_message("Show strings requested (not implemented yet)");
}

void MainWindow::on_action_show_imports_triggered() {
    log_message("Show imports requested (not implemented yet)");
}

void MainWindow::on_action_show_exports_triggered() {
    log_message("Show exports requested (not implemented yet)");
}

void MainWindow::update_debug_state() {
    // Update debug state display
}

void MainWindow::on_breakpoint_hit(uint64_t address) {
    log_message(QString("Breakpoint hit at 0x%1").arg(address, 0, 16));
}

void MainWindow::on_function_selected(uint64_t address) {
    navigate_to_address(address);
}

void MainWindow::on_address_double_clicked(uint64_t address) {
    navigate_to_address(address);
}

void MainWindow::refresh_views() {
    // Refresh dynamic content
}

void MainWindow::update_title() {
    QString title = "Advanced Debugger v1.0.0";
    if (!current_filename.isEmpty()) {
        title += " - " + QFileInfo(current_filename).fileName();
    }
    setWindowTitle(title);
}

void MainWindow::update_status() {
    if (current_filename.isEmpty()) {
        status_label->setText("Ready");
        architecture_label->setText("");
    } else {
        status_label->setText("File loaded");
        architecture_label->setText(QString::fromStdString(disassembler->get_architecture_name()));
    }
    
    debug_state_label->setText("Not Running");
}

void MainWindow::populate_functions_tree() {
    functions_tree->clear();
    
    if (elf_parser->is_valid_elf()) {
        std::vector<Symbol> functions = elf_parser->get_functions();
        for (const auto& func : functions) {
            QTreeWidgetItem* item = new QTreeWidgetItem();
            item->setText(0, QString::fromStdString(func.name));
            item->setText(1, QString("0x%1").arg(func.address, 0, 16));
            functions_tree->addTopLevelItem(item);
        }
    }
}

void MainWindow::populate_symbols_tree() {
    symbols_tree->clear();
    
    if (elf_parser->is_valid_elf()) {
        std::vector<Symbol> symbols = elf_parser->get_symbols();
        for (const auto& symbol : symbols) {
            QTreeWidgetItem* item = new QTreeWidgetItem();
            item->setText(0, QString::fromStdString(symbol.name));
            symbols_tree->addTopLevelItem(item);
        }
    }
}

void MainWindow::populate_sections_table() {
    sections_table->setRowCount(0);
    
    if (elf_parser->is_valid_elf()) {
        std::vector<Section> sections = elf_parser->get_sections();
        sections_table->setRowCount(sections.size());
        
        for (size_t i = 0; i < sections.size(); ++i) {
            const auto& section = sections[i];
            sections_table->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(section.name)));
            sections_table->setItem(i, 1, new QTableWidgetItem(QString("0x%1").arg(section.address, 0, 16)));
            sections_table->setItem(i, 2, new QTableWidgetItem(QString::number(section.size)));
            sections_table->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(section.type)));
        }
    }
}

void MainWindow::populate_imports_table() {
    imports_table->setRowCount(0);
    
    if (elf_parser->is_valid_elf()) {
        std::vector<Import> imports = elf_parser->get_imports();
        imports_table->setRowCount(imports.size());
        
        for (size_t i = 0; i < imports.size(); ++i) {
            const auto& import = imports[i];
            imports_table->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(import.name)));
            imports_table->setItem(i, 1, new QTableWidgetItem(QString("0x%1").arg(import.address, 0, 16)));
            imports_table->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(import.type)));
        }
    }
}

void MainWindow::populate_exports_table() {
    exports_table->setRowCount(0);
    
    if (elf_parser->is_valid_elf()) {
        std::vector<Export> exports = elf_parser->get_exports();
        exports_table->setRowCount(exports.size());
        
        for (size_t i = 0; i < exports.size(); ++i) {
            const auto& exp = exports[i];
            exports_table->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(exp.name)));
            exports_table->setItem(i, 1, new QTableWidgetItem(QString("0x%1").arg(exp.address, 0, 16)));
            exports_table->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(exp.type)));
        }
    }
}

void MainWindow::populate_strings_view() {
    strings_view->clear();
    
    if (elf_parser->is_valid_elf()) {
        std::vector<uint8_t> code_data = elf_parser->get_code_section_data();
        if (!code_data.empty()) {
            std::vector<std::string> strings = disassembler->extract_strings(code_data.data(), code_data.size());
            
            QString content;
            for (const auto& str : strings) {
                content += QString::fromStdString(str) + "\n";
            }
            strings_view->setPlainText(content);
        }
    }
}

void MainWindow::log_message(const QString& message) {
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    log_view->append(QString("[%1] %2").arg(timestamp, message));
}

void MainWindow::show_error(const QString& message) {
    QMessageBox::critical(this, "Error", message);
    log_message("ERROR: " + message);
}

void MainWindow::show_info(const QString& message) {
    QMessageBox::information(this, "Information", message);
}

bool MainWindow::confirm_action(const QString& message) {
    return QMessageBox::question(this, "Confirm", message,
                               QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes;
}

void MainWindow::navigate_to_address(uint64_t address) {
    current_address = address;
    disassembly_view->highlight_instruction(address);
    log_message(QString("Navigated to address 0x%1").arg(address, 0, 16));
}

void MainWindow::highlight_current_instruction(uint64_t address) {
    disassembly_view->highlight_instruction(address);
}

void MainWindow::update_debug_controls() {
    bool debugging = (current_debug_state == DebuggerState::RUNNING || 
                     current_debug_state == DebuggerState::PAUSED);
    bool paused = (current_debug_state == DebuggerState::PAUSED);
    
    continue_button->setEnabled(paused);
    pause_button->setEnabled(current_debug_state == DebuggerState::RUNNING);
    step_into_button->setEnabled(paused);
    step_over_button->setEnabled(paused);
    step_out_button->setEnabled(paused);
}

void MainWindow::save_settings() {
    QSettings settings;
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
}

void MainWindow::load_settings() {
    QSettings settings;
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
}

// Placeholder implementations for custom view classes
DisassemblyView::DisassemblyView(QWidget* parent) : QTextEdit(parent), highlighted_address(0) {
    setFont(QFont("Courier", 10));
    setReadOnly(true);
}

void DisassemblyView::set_instructions(const std::vector<Instruction>& instructions) {
    current_instructions = instructions;
    
    QString content;
    for (const auto& insn : instructions) {
        content += QString("0x%1: %2 %3\n")
                  .arg(insn.address, 8, 16, QChar('0'))
                  .arg(QString::fromStdString(insn.mnemonic), -8)
                  .arg(QString::fromStdString(insn.operands));
    }
    
    setPlainText(content);
}

void DisassemblyView::highlight_instruction(uint64_t address) {
    highlighted_address = address;
    // Implementation for highlighting would go here
}

void DisassemblyView::clear_highlight() {
    highlighted_address = 0;
}

void DisassemblyView::mouseDoubleClickEvent(QMouseEvent* event) {
    QTextEdit::mouseDoubleClickEvent(event);
    // Implementation would extract address from current line
}

void DisassemblyView::contextMenuEvent(QContextMenuEvent* event) {
    QTextEdit::contextMenuEvent(event);
    // Implementation would show context menu
}

DecompilerView::DecompilerView(QWidget* parent) : QTextEdit(parent) {
    setFont(QFont("Courier", 10));
    setReadOnly(true);
}

void DecompilerView::set_decompiled_code(const std::string& code) {
    setPlainText(QString::fromStdString(code));
}

void DecompilerView::clear_code() {
    clear();
}

void DecompilerView::setup_syntax_highlighting() {
    // Placeholder for syntax highlighting
}

MemoryView::MemoryView(QWidget* parent) : QTableWidget(parent), base_address(0) {
    setColumnCount(17); // Address + 16 bytes
    setHorizontalHeaderLabels({"Address", "00", "01", "02", "03", "04", "05", "06", "07",
                              "08", "09", "0A", "0B", "0C", "0D", "0E", "0F"});
}

void MemoryView::set_memory_data(uint64_t start_address, const std::vector<uint8_t>& data) {
    base_address = start_address;
    memory_data = data;
    update_display();
}

void MemoryView::refresh_memory(uint64_t address, size_t size) {
    // Placeholder
}

void MemoryView::update_display() {
    // Implementation would display memory in hex format
}

RegistersView::RegistersView(QWidget* parent) : QTableWidget(parent) {
    setup_columns();
}

void RegistersView::set_registers(const std::vector<Register>& registers) {
    current_registers = registers;
    // Implementation would display registers
}

void RegistersView::highlight_modified_registers() {
    // Implementation would highlight changed registers
}

void RegistersView::setup_columns() {
    setColumnCount(3);
    setHorizontalHeaderLabels({"Register", "Value", "Size"});
}

BreakpointView::BreakpointView(QWidget* parent) : QTableWidget(parent) {
    setup_columns();
}

void BreakpointView::set_breakpoints(const std::vector<Breakpoint>& breakpoints) {
    // Implementation would display breakpoints
}

void BreakpointView::add_breakpoint(const Breakpoint& bp) {
    // Implementation would add breakpoint to table
}

void BreakpointView::remove_breakpoint(uint64_t address) {
    // Implementation would remove breakpoint from table
}

void BreakpointView::setup_columns() {
    setColumnCount(4);
    setHorizontalHeaderLabels({"Address", "Type", "Enabled", "Condition"});
}

void BreakpointView::update_breakpoint_list() {
    // Implementation would refresh breakpoint display
}

} // namespace debugger 