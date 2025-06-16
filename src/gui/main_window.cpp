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
    
    // Initialize refresh functionality (timer removed for simplicity)
    
    // Load settings
    load_settings();
    
    // Set window properties
    setWindowTitle("Advanced Debugger v1.0.0");
    setMinimumSize(1000, 700);
    resize(1400, 900);
    
    // Apply dark theme
    apply_dark_theme();
}

MainWindow::~MainWindow() {
    save_settings();
}

void MainWindow::apply_dark_theme() {
    // Dark theme stylesheet
    QString darkStyle = R"(
        QMainWindow {
            background-color: #2b2b2b;
            color: #ffffff;
        }
        
        QMenuBar {
            background-color: #3c3c3c;
            color: #ffffff;
            border-bottom: 1px solid #555555;
        }
        
        QMenuBar::item {
            background-color: transparent;
            padding: 4px 8px;
        }
        
        QMenuBar::item:selected {
            background-color: #4a90e2;
        }
        
        QMenu {
            background-color: #3c3c3c;
            color: #ffffff;
            border: 1px solid #555555;
        }
        
        QMenu::item {
            padding: 4px 16px;
        }
        
        QMenu::item:selected {
            background-color: #4a90e2;
        }
        
        QTextEdit {
            background-color: #1e1e1e;
            color: #d4d4d4;
            border: 1px solid #555555;
            font-family: 'Consolas', 'Monaco', 'Courier New', monospace;
            font-size: 11px;
            selection-background-color: #264f78;
        }
        
        QTreeWidget {
            background-color: #252526;
            color: #cccccc;
            border: 1px solid #555555;
            alternate-background-color: #2d2d30;
        }
        
        QTreeWidget::item {
            padding: 4px;
            border-bottom: 1px solid #404040;
        }
        
        QTreeWidget::item:selected {
            background-color: #0e639c;
        }
        
        QTreeWidget::item:hover {
            background-color: #383838;
        }
        
        QTableWidget {
            background-color: #252526;
            color: #cccccc;
            border: 1px solid #555555;
            gridline-color: #404040;
        }
        
        QTableWidget::item {
            padding: 4px;
            border-bottom: 1px solid #404040;
        }
        
        QTableWidget::item:selected {
            background-color: #0e639c;
        }
        
        QHeaderView::section {
            background-color: #3c3c3c;
            color: #ffffff;
            border: 1px solid #555555;
            padding: 4px;
        }
        
        QTabWidget::pane {
            border: 1px solid #555555;
            background-color: #2b2b2b;
        }
        
        QTabBar::tab {
            background-color: #3c3c3c;
            color: #ffffff;
            padding: 6px 12px;
            margin-right: 2px;
            border-top-left-radius: 4px;
            border-top-right-radius: 4px;
        }
        
        QTabBar::tab:selected {
            background-color: #4a90e2;
        }
        
        QTabBar::tab:hover {
            background-color: #505050;
        }
        
        QSplitter::handle {
            background-color: #555555;
        }
        
        QSplitter::handle:horizontal {
            width: 3px;
        }
        
        QSplitter::handle:vertical {
            height: 3px;
        }
        
        QLabel {
            color: #ffffff;
        }
        
        QStatusBar {
            background-color: #3c3c3c;
            color: #ffffff;
            border-top: 1px solid #555555;
        }
        
        /* Scrollbars */
        QScrollBar:vertical {
            background-color: #2b2b2b;
            width: 12px;
            border-radius: 6px;
        }
        
        QScrollBar::handle:vertical {
            background-color: #555555;
            border-radius: 6px;
            min-height: 20px;
        }
        
        QScrollBar::handle:vertical:hover {
            background-color: #777777;
        }
        
        QScrollBar:horizontal {
            background-color: #2b2b2b;
            height: 12px;
            border-radius: 6px;
        }
        
        QScrollBar::handle:horizontal {
            background-color: #555555;
            border-radius: 6px;
            min-width: 20px;
        }
        
        QScrollBar::handle:horizontal:hover {
            background-color: #777777;
        }
        
        QScrollBar::add-line, QScrollBar::sub-line {
            background-color: transparent;
        }
    )";
    
    setStyleSheet(darkStyle);
}

void MainWindow::setup_ui() {
    QWidget* central_widget = new QWidget(this);
    setCentralWidget(central_widget);
    
    QVBoxLayout* main_layout = new QVBoxLayout(central_widget);
    main_layout->setContentsMargins(5, 5, 5, 5);
    main_layout->setSpacing(5);
    
    // Create main horizontal splitter
    QSplitter* main_splitter = new QSplitter(Qt::Horizontal, this);
    main_layout->addWidget(main_splitter);
    
    // Left panel - File analysis
    setup_left_panel(main_splitter);
    
    // Center panel - Disassembly and decompiler
    setup_center_panel(main_splitter);
    
    // Right panel - Debug info
    setup_right_panel(main_splitter);
    
    // Set splitter proportions
    main_splitter->setSizes({300, 800, 350});
    
    // Status bar
    setup_status_bar();
}

void MainWindow::setup_left_panel(QSplitter* parent) {
    QTabWidget* left_tabs = new QTabWidget();
    
    // Functions tree
    functions_tree = new QTreeWidget();
    functions_tree->setHeaderLabel("Functions");
    functions_tree->setAlternatingRowColors(true);
    left_tabs->addTab(functions_tree, "Functions");
    
    // Symbols tree  
    symbols_tree = new QTreeWidget();
    symbols_tree->setHeaderLabel("Symbols");
    symbols_tree->setAlternatingRowColors(true);
    left_tabs->addTab(symbols_tree, "Symbols");
    
    // Sections table
    sections_table = new QTableWidget();
    sections_table->setColumnCount(4);
    sections_table->setHorizontalHeaderLabels({"Name", "Address", "Size", "Type"});
    sections_table->horizontalHeader()->setStretchLastSection(true);
    sections_table->setAlternatingRowColors(true);
    sections_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    left_tabs->addTab(sections_table, "Sections");
    
    // Strings view
    strings_view = new QTextEdit();
    strings_view->setReadOnly(true);
    left_tabs->addTab(strings_view, "Strings");
    
    parent->addWidget(left_tabs);
}

void MainWindow::setup_center_panel(QSplitter* parent) {
    QTabWidget* center_tabs = new QTabWidget();
    
    // Disassembly view
    disassembly_view = new DisassemblyView();
    center_tabs->addTab(disassembly_view, "Disassembly");
    
    // Decompiler view
    decompiler_view = new DecompilerView();
    center_tabs->addTab(decompiler_view, "Decompiler");
    
    parent->addWidget(center_tabs);
}

void MainWindow::setup_right_panel(QSplitter* parent) {
    QTabWidget* right_tabs = new QTabWidget();
    
    // Registers view
    registers_view = new RegistersView();
    right_tabs->addTab(registers_view, "Registers");
    
    // Memory view
    memory_view = new MemoryView();
    right_tabs->addTab(memory_view, "Memory");
    
    // Breakpoints view
    breakpoint_view = new BreakpointView();
    right_tabs->addTab(breakpoint_view, "Breakpoints");
    
    // Log view
    log_view = new QTextEdit();
    log_view->setReadOnly(true);
    // Note: QTextEdit doesn't have setMaximumBlockCount, this was a mistake
    right_tabs->addTab(log_view, "Log");
    
    parent->addWidget(right_tabs);
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
    QAction* start_action = debug_menu->addAction("&Start Debugging (F5)");
    start_action->setShortcut(QKeySequence("F5"));
    connect(start_action, &QAction::triggered, this, &MainWindow::on_action_start_debug_triggered);
    
    QAction* step_into_action = debug_menu->addAction("Step &Into (F11)");
    step_into_action->setShortcut(QKeySequence("F11"));
    connect(step_into_action, &QAction::triggered, this, &MainWindow::on_action_step_into_triggered);
    
    QAction* step_over_action = debug_menu->addAction("Step &Over (F10)");
    step_over_action->setShortcut(QKeySequence("F10"));
    connect(step_over_action, &QAction::triggered, this, &MainWindow::on_action_step_over_triggered);
    
    // Tools menu
    QMenu* tools_menu = menu_bar->addMenu("&Tools");
    
    QAction* analyze_action = tools_menu->addAction("&Analyze Functions");
    connect(analyze_action, &QAction::triggered, this, &MainWindow::on_action_analyze_functions_triggered);
    
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
    status_label = new QLabel("Ready");
    architecture_label = new QLabel("");
    debug_state_label = new QLabel("Not Running");
    
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
    strings_view->clear();
    
    update_title();
    update_status();
    
    log_message("File closed");
}

void MainWindow::on_action_open_triggered() {
    QString filename = QFileDialog::getOpenFileName(this,
        "Open Binary File", QDir::homePath(), 
        "Executable Files (*);;ELF Files (*.elf);;All Files (*)");
    
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
        "<h3>Advanced Debugger v1.0.0</h3>"
        "<p>A comprehensive GUI-based debugger with disassembly, "
        "decompilation, and debugging capabilities.</p>"
        "<p><b>Features:</b></p>"
        "<ul>"
        "<li>Multi-architecture disassembly (x86, x86-64, ARM)</li>"
        "<li>ELF binary analysis and parsing</li>"
        "<li>Basic decompilation to C-like pseudocode</li>"
        "<li>Process debugging with breakpoints</li>"
        "<li>Memory and register inspection</li>"
        "</ul>"
        "<p>Built with Qt5, Capstone Engine, and custom ELF parsing.</p>");
}

void MainWindow::on_action_start_debug_triggered() {
    if (current_filename.isEmpty()) {
        QMessageBox::warning(this, "No File", "Please load a binary file first.");
        return;
    }
    
    log_message("Starting debug session...");
    
    // Try to load the executable into the debugger
    if (debugger_engine->load_executable(current_filename.toStdString())) {
        if (debugger_engine->start_process()) {
            current_debug_state = DebuggerState::PAUSED;
            debug_state_label->setText("Paused");
            log_message("Debug session started successfully");
            QMessageBox::information(this, "Debug Started", "Debug session started. The process is paused at entry point.");
        } else {
            log_message("ERROR: Failed to start debug process");
            QMessageBox::critical(this, "Debug Error", "Failed to start debug process: " + QString::fromStdString(debugger_engine->get_last_error()));
        }
    } else {
        log_message("ERROR: Failed to load executable");
        QMessageBox::critical(this, "Debug Error", "Failed to load executable for debugging");
    }
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
    if (current_debug_state != DebuggerState::PAUSED) {
        QMessageBox::warning(this, "Not Debugging", "No active debug session. Start debugging first.");
        return;
    }
    
    log_message("Stepping into...");
    if (debugger_engine->step_into()) {
        log_message("Step completed");
        // Update register and memory views here
    } else {
        log_message("ERROR: Step failed");
        QMessageBox::warning(this, "Step Error", "Failed to step: " + QString::fromStdString(debugger_engine->get_last_error()));
    }
}

void MainWindow::on_action_step_over_triggered() {
    if (current_debug_state != DebuggerState::PAUSED) {
        QMessageBox::warning(this, "Not Debugging", "No active debug session. Start debugging first.");
        return;
    }
    
    log_message("Stepping over...");
    if (debugger_engine->step_over()) {
        log_message("Step over completed");
    } else {
        log_message("ERROR: Step over failed");
        QMessageBox::warning(this, "Step Error", "Failed to step over: " + QString::fromStdString(debugger_engine->get_last_error()));
    }
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
    if (!elf_parser->is_valid_elf()) {
        QMessageBox::warning(this, "No File", "Please load a binary file first.");
        return;
    }
    
    log_message("Analyzing functions...");
    
    // Get disassembled instructions
    std::vector<uint8_t> code_data = elf_parser->get_code_section_data();
    if (!code_data.empty()) {
        uint64_t entry_point = elf_parser->get_entry_point();
        std::vector<Instruction> instructions = disassembler->disassemble(
            code_data.data(), code_data.size(), entry_point);
        
        // Analyze functions
        std::vector<Function> functions = disassembler->analyze_functions(instructions);
        
        log_message(QString("Found %1 functions").arg(functions.size()));
        
        // Display results
        QString result = QString("Function Analysis Results:\n\n");
        for (const auto& func : functions) {
            result += QString("Function: %1\n").arg(QString::fromStdString(func.name));
            result += QString("  Start: 0x%1\n").arg(func.start_address, 0, 16);
            result += QString("  End: 0x%1\n").arg(func.end_address, 0, 16);
            result += QString("  Instructions: %1\n\n").arg(func.instructions.size());
        }
        
        // Show in a new dialog or update decompiler view
        decompiler_view->setPlainText(result);
    }
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
        status_label->setText("File loaded: " + QFileInfo(current_filename).fileName());
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
            item->setToolTip(0, QString("Address: 0x%1, Size: %2 bytes").arg(func.address, 0, 16).arg(func.size));
            functions_tree->addTopLevelItem(item);
        }
        
        if (functions.empty()) {
            QTreeWidgetItem* item = new QTreeWidgetItem();
            item->setText(0, "No functions found");
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
            item->setToolTip(0, QString("Type: %1, Address: 0x%2").arg(QString::fromStdString(symbol.type)).arg(symbol.address, 0, 16));
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
        
        sections_table->resizeColumnsToContents();
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
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
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
    setFont(QFont("Consolas", 10));
    setReadOnly(true);
    setLineWrapMode(QTextEdit::NoWrap);
}

void DisassemblyView::set_instructions(const std::vector<Instruction>& instructions) {
    current_instructions = instructions;
    
    QString content;
    content += QString("Disassembly (%1 instructions):\n\n").arg(instructions.size());
    
    for (const auto& insn : instructions) {
        QString line = QString("0x%1:  %2    %3")
                      .arg(insn.address, 8, 16, QChar('0'))
                      .arg(QString::fromStdString(insn.mnemonic), -8)
                      .arg(QString::fromStdString(insn.operands));
        
        // Add instruction type indicators
        if (insn.is_call) {
            line += "    ; CALL";
        } else if (insn.is_jump) {
            line += "    ; JUMP";
        } else if (insn.is_return) {
            line += "    ; RETURN";
        }
        
        content += line + "\n";
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
    setFont(QFont("Consolas", 10));
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
    QStringList headers;
    headers << "Address";
    for (int i = 0; i < 16; ++i) {
        headers << QString("%1").arg(i, 2, 16, QChar('0')).toUpper();
    }
    setHorizontalHeaderLabels(headers);
    setAlternatingRowColors(true);
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
    setColumnCount(3);
    setHorizontalHeaderLabels({"Register", "Value", "Size"});
    setAlternatingRowColors(true);
    horizontalHeader()->setStretchLastSection(true);
}

void RegistersView::set_registers(const std::vector<Register>& registers) {
    previous_registers = registers;
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
    setColumnCount(4);
    setHorizontalHeaderLabels({"Address", "Type", "Enabled", "Condition"});
    setAlternatingRowColors(true);
    horizontalHeader()->setStretchLastSection(true);
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