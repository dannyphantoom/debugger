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
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QDialog>
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
    QWidget* right_widget = new QWidget();
    QVBoxLayout* right_layout = new QVBoxLayout(right_widget);
    right_layout->setContentsMargins(5, 5, 5, 5);
    
    // Debug control buttons
    QWidget* debug_controls = new QWidget();
    QHBoxLayout* controls_layout = new QHBoxLayout(debug_controls);
    controls_layout->setContentsMargins(0, 0, 0, 0);
    
    continue_button = new QPushButton("Continue");
    pause_button = new QPushButton("Pause");
    step_into_button = new QPushButton("Step Into");
    step_over_button = new QPushButton("Step Over");
    step_out_button = new QPushButton("Step Out");
    
    continue_button->setToolTip("Continue execution (F5)");
    pause_button->setToolTip("Pause execution");
    step_into_button->setToolTip("Step into function (F11)");
    step_over_button->setToolTip("Step over instruction (F10)");
    step_out_button->setToolTip("Step out of function");
    
    controls_layout->addWidget(continue_button);
    controls_layout->addWidget(pause_button);
    controls_layout->addSeparator();
    controls_layout->addWidget(step_into_button);
    controls_layout->addWidget(step_over_button);
    controls_layout->addWidget(step_out_button);
    controls_layout->addStretch();
    
    right_layout->addWidget(debug_controls);
    
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
    right_tabs->addTab(log_view, "Log");
    
    right_layout->addWidget(right_tabs);
    parent->addWidget(right_widget);
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
        // Try to get address from tooltip or item data
        QString tooltip = item->toolTip(0);
        if (tooltip.contains("Address:")) {
            QString address_str = tooltip.split("Address: ")[1].split(",")[0];
            if (address_str.startsWith("0x")) {
                address_str.remove(0, 2);
            }
            
            bool ok;
            uint64_t address = address_str.toULongLong(&ok, 16);
            if (ok) {
                navigate_to_address(address);
            }
        }
    });
    
    connect(symbols_tree, &QTreeWidget::itemDoubleClicked, [this](QTreeWidgetItem* item) {
        QString tooltip = item->toolTip(0);
        if (tooltip.contains("Address:")) {
            QString address_str = tooltip.split("Address: ")[1].split(")")[0];
            if (address_str.startsWith("0x")) {
                address_str.remove(0, 2);
            }
            
            bool ok;
            uint64_t address = address_str.toULongLong(&ok, 16);
            if (ok) {
                navigate_to_address(address);
            }
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
    bool ok;
    QString pid_text = QInputDialog::getText(this, "Attach to Process", 
                                           "Enter Process ID (PID):", 
                                           QLineEdit::Normal, "", &ok);
    
    if (ok && !pid_text.isEmpty()) {
        pid_t pid = pid_text.toInt(&ok);
        if (ok && pid > 0) {
            log_message(QString("Attempting to attach to process %1...").arg(pid));
            
            if (debugger_engine->attach_to_process(pid)) {
                current_debug_state = DebuggerState::PAUSED;
                debug_state_label->setText("Attached");
                update_debug_controls();
                log_message(QString("Successfully attached to process %1").arg(pid));
                QMessageBox::information(this, "Process Attached", 
                                       QString("Successfully attached to process %1").arg(pid));
            } else {
                log_message("ERROR: Failed to attach to process");
                QMessageBox::critical(this, "Attach Error", 
                                    "Failed to attach to process: " + 
                                    QString::fromStdString(debugger_engine->get_last_error()));
            }
        } else {
            QMessageBox::warning(this, "Invalid PID", "Please enter a valid process ID.");
        }
    }
}

void MainWindow::on_action_continue_triggered() {
    if (current_debug_state != DebuggerState::PAUSED) {
        QMessageBox::warning(this, "Not Paused", "Process is not paused. Start debugging first.");
        return;
    }
    
    log_message("Continuing execution...");
    if (debugger_engine->continue_execution()) {
        current_debug_state = DebuggerState::RUNNING;
        debug_state_label->setText("Running");
        update_debug_controls();
        log_message("Execution continued");
    } else {
        log_message("ERROR: Failed to continue execution");
        QMessageBox::critical(this, "Continue Error", 
                            "Failed to continue execution: " + 
                            QString::fromStdString(debugger_engine->get_last_error()));
    }
}

void MainWindow::on_action_pause_triggered() {
    if (current_debug_state != DebuggerState::RUNNING) {
        QMessageBox::warning(this, "Not Running", "Process is not running.");
        return;
    }
    
    log_message("Pausing execution...");
    if (debugger_engine->pause_execution()) {
        current_debug_state = DebuggerState::PAUSED;
        debug_state_label->setText("Paused");
        update_debug_controls();
        refresh_views();
        log_message("Execution paused");
    } else {
        log_message("ERROR: Failed to pause execution");
        QMessageBox::critical(this, "Pause Error", 
                            "Failed to pause execution: " + 
                            QString::fromStdString(debugger_engine->get_last_error()));
    }
}

void MainWindow::on_action_stop_triggered() {
    if (current_debug_state == DebuggerState::NOT_RUNNING) {
        QMessageBox::information(this, "Not Running", "No debug session is active.");
        return;
    }
    
    if (confirm_action("Are you sure you want to stop the debug session?")) {
        log_message("Stopping debug session...");
        if (debugger_engine->stop_execution()) {
            current_debug_state = DebuggerState::NOT_RUNNING;
            debug_state_label->setText("Stopped");
            update_debug_controls();
            
            // Clear debug-specific views
            registers_view->set_registers({});
            memory_view->set_memory_data(0, {});
            
            log_message("Debug session stopped");
        } else {
            log_message("ERROR: Failed to stop execution");
            QMessageBox::critical(this, "Stop Error", 
                                "Failed to stop execution: " + 
                                QString::fromStdString(debugger_engine->get_last_error()));
        }
    }
}

void MainWindow::on_action_step_into_triggered() {
    if (current_debug_state != DebuggerState::PAUSED) {
        QMessageBox::warning(this, "Not Debugging", "No active debug session. Start debugging first.");
        return;
    }
    
    log_message("Stepping into...");
    if (debugger_engine->step_into()) {
        log_message("Step completed");
        refresh_views();
        
        // Update current address
        current_address = debugger_engine->get_instruction_pointer();
        highlight_current_instruction(current_address);
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
        refresh_views();
        
        // Update current address
        current_address = debugger_engine->get_instruction_pointer();
        highlight_current_instruction(current_address);
    } else {
        log_message("ERROR: Step over failed");
        QMessageBox::warning(this, "Step Error", "Failed to step over: " + QString::fromStdString(debugger_engine->get_last_error()));
    }
}

void MainWindow::on_action_step_out_triggered() {
    if (current_debug_state != DebuggerState::PAUSED) {
        QMessageBox::warning(this, "Not Debugging", "No active debug session. Start debugging first.");
        return;
    }
    
    log_message("Stepping out of function...");
    if (debugger_engine->step_out()) {
        log_message("Step out completed");
        refresh_views();
        
        // Update current address
        current_address = debugger_engine->get_instruction_pointer();
        highlight_current_instruction(current_address);
    } else {
        log_message("ERROR: Step out failed");
        QMessageBox::warning(this, "Step Error", 
                           "Failed to step out: " + 
                           QString::fromStdString(debugger_engine->get_last_error()));
    }
}

void MainWindow::on_action_go_to_address_triggered() {
    bool ok;
    QString address_text = QInputDialog::getText(this, "Go to Address", 
                                               "Enter address (hex format, e.g., 0x401000):", 
                                               QLineEdit::Normal, "", &ok);
    
    if (ok && !address_text.isEmpty()) {
        // Remove 0x prefix if present
        QString clean_address = address_text;
        if (clean_address.startsWith("0x", Qt::CaseInsensitive)) {
            clean_address.remove(0, 2);
        }
        
        uint64_t address = clean_address.toULongLong(&ok, 16);
        if (ok) {
            log_message(QString("Navigating to address 0x%1").arg(address, 0, 16));
            navigate_to_address(address);
            
            // If debugging, also try to read memory at that address
            if (current_debug_state == DebuggerState::PAUSED) {
                std::vector<uint8_t> memory = debugger_engine->read_memory(address, 256);
                if (!memory.empty()) {
                    memory_view->set_memory_data(address, memory);
                }
            }
        } else {
            QMessageBox::warning(this, "Invalid Address", 
                               "Please enter a valid hexadecimal address (e.g., 0x401000 or 401000).");
        }
    }
}

void MainWindow::on_action_find_triggered() {
    bool ok;
    QString search_text = QInputDialog::getText(this, "Find", 
                                              "Enter text to search for:", 
                                              QLineEdit::Normal, "", &ok);
    
    if (ok && !search_text.isEmpty()) {
        log_message(QString("Searching for: %1").arg(search_text));
        
        // Get the current active tab/view and search in it
        QTabWidget* center_tabs = dynamic_cast<QTabWidget*>(disassembly_view->parent());
        if (center_tabs) {
            int current_tab = center_tabs->currentIndex();
            
            if (current_tab == 0) { // Disassembly view
                QTextDocument* doc = disassembly_view->document();
                QTextCursor cursor = doc->find(search_text, disassembly_view->textCursor());
                
                if (!cursor.isNull()) {
                    disassembly_view->setTextCursor(cursor);
                    disassembly_view->ensureCursorVisible();
                    log_message("Found text in disassembly view");
                } else {
                    QMessageBox::information(this, "Find", "Text not found in disassembly view.");
                    log_message("Text not found in disassembly view");
                }
            } else if (current_tab == 1) { // Decompiler view
                QTextDocument* doc = decompiler_view->document();
                QTextCursor cursor = doc->find(search_text, decompiler_view->textCursor());
                
                if (!cursor.isNull()) {
                    decompiler_view->setTextCursor(cursor);
                    decompiler_view->ensureCursorVisible();
                    log_message("Found text in decompiler view");
                } else {
                    QMessageBox::information(this, "Find", "Text not found in decompiler view.");
                    log_message("Text not found in decompiler view");
                }
            }
        }
    }
}

void MainWindow::on_action_toggle_breakpoint_triggered() {
    // Use current address if available, otherwise ask user for address
    uint64_t address = current_address;
    
    if (address == 0) {
        bool ok;
        QString address_text = QInputDialog::getText(this, "Toggle Breakpoint", 
                                                   "Enter address for breakpoint (hex format):", 
                                                   QLineEdit::Normal, "", &ok);
        
        if (!ok || address_text.isEmpty()) {
            return;
        }
        
        QString clean_address = address_text;
        if (clean_address.startsWith("0x", Qt::CaseInsensitive)) {
            clean_address.remove(0, 2);
        }
        
        address = clean_address.toULongLong(&ok, 16);
        if (!ok) {
            QMessageBox::warning(this, "Invalid Address", "Please enter a valid hexadecimal address.");
            return;
        }
    }
    
    // Check if breakpoint already exists at this address
    std::vector<Breakpoint> breakpoints = debugger_engine->get_breakpoints();
    bool breakpoint_exists = false;
    
    for (const auto& bp : breakpoints) {
        if (bp.address == address) {
            breakpoint_exists = true;
            break;
        }
    }
    
    if (breakpoint_exists) {
        // Remove breakpoint
        log_message(QString("Removing breakpoint at 0x%1").arg(address, 0, 16));
        if (debugger_engine->remove_breakpoint(address)) {
            log_message("Breakpoint removed successfully");
        } else {
            log_message("ERROR: Failed to remove breakpoint");
            QMessageBox::critical(this, "Breakpoint Error", 
                                "Failed to remove breakpoint: " + 
                                QString::fromStdString(debugger_engine->get_last_error()));
        }
    } else {
        // Add breakpoint
        log_message(QString("Adding breakpoint at 0x%1").arg(address, 0, 16));
        if (debugger_engine->add_breakpoint(address)) {
            log_message("Breakpoint added successfully");
        } else {
            log_message("ERROR: Failed to add breakpoint");
            QMessageBox::critical(this, "Breakpoint Error", 
                                "Failed to add breakpoint: " + 
                                QString::fromStdString(debugger_engine->get_last_error()));
        }
    }
    
    // Update breakpoint view
    breakpoint_view->set_breakpoints(debugger_engine->get_breakpoints());
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
    if (!elf_parser->is_valid_elf()) {
        QMessageBox::warning(this, "No File", "Please load a binary file first.");
        return;
    }
    
    log_message("Extracting strings from binary...");
    
    // Extract strings from different sections
    QString strings_content = "=== STRINGS ANALYSIS ===\n\n";
    
    // Get strings from code section
    std::vector<uint8_t> code_data = elf_parser->get_code_section_data();
    if (!code_data.empty()) {
        std::vector<std::string> code_strings = disassembler->extract_strings(code_data.data(), code_data.size());
        
        strings_content += QString("Code Section Strings (%1 found):\n").arg(code_strings.size());
        strings_content += "-----------------------------------\n";
        
        for (size_t i = 0; i < code_strings.size(); ++i) {
            strings_content += QString("[%1] %2\n").arg(i + 1).arg(QString::fromStdString(code_strings[i]));
        }
        strings_content += "\n";
    }
    
    // Get strings from data sections
    std::vector<Section> sections = elf_parser->get_sections();
    for (const auto& section : sections) {
        if (section.name == ".rodata" || section.name == ".data" || section.name == ".bss") {
            if (!section.data.empty()) {
                std::vector<std::string> section_strings = disassembler->extract_strings(
                    section.data.data(), section.data.size());
                
                if (!section_strings.empty()) {
                    strings_content += QString("%1 Section Strings (%2 found):\n")
                                     .arg(QString::fromStdString(section.name))
                                     .arg(section_strings.size());
                    strings_content += "-----------------------------------\n";
                    
                    for (size_t i = 0; i < section_strings.size(); ++i) {
                        strings_content += QString("[%1] %2\n").arg(i + 1).arg(QString::fromStdString(section_strings[i]));
                    }
                    strings_content += "\n";
                }
            }
        }
    }
    
    // Update the strings view
    strings_view->setPlainText(strings_content);
    
    // Switch to the strings tab
    QTabWidget* left_tabs = dynamic_cast<QTabWidget*>(strings_view->parent());
    if (left_tabs) {
        left_tabs->setCurrentWidget(strings_view);
    }
    
    log_message("String extraction completed");
}

void MainWindow::on_action_show_imports_triggered() {
    if (!elf_parser->is_valid_elf()) {
        QMessageBox::warning(this, "No File", "Please load a binary file first.");
        return;
    }
    
    log_message("Analyzing imports...");
    
    std::vector<Import> imports = elf_parser->get_imports();
    
    QString imports_content = "=== IMPORTS ANALYSIS ===\n\n";
    imports_content += QString("Total imports found: %1\n\n").arg(imports.size());
    
    if (imports.empty()) {
        imports_content += "No imports found in this binary.\n";
    } else {
        imports_content += "Library\t\tFunction\t\tAddress\t\tType\n";
        imports_content += "================================================================\n";
        
        for (const auto& import : imports) {
            imports_content += QString("%1\t\t%2\t\t0x%3\t\t%4\n")
                             .arg(QString::fromStdString(import.library), -15)
                             .arg(QString::fromStdString(import.name), -20)
                             .arg(import.address, 8, 16, QChar('0'))
                             .arg(QString::fromStdString(import.type));
        }
    }
    
    // Create a new dialog to show imports
    QDialog* imports_dialog = new QDialog(this);
    imports_dialog->setWindowTitle("Imports Analysis");
    imports_dialog->setModal(false);
    imports_dialog->resize(800, 600);
    
    QVBoxLayout* layout = new QVBoxLayout(imports_dialog);
    QTextEdit* imports_text = new QTextEdit();
    imports_text->setPlainText(imports_content);
    imports_text->setReadOnly(true);
    imports_text->setFont(QFont("Consolas", 10));
    
    layout->addWidget(imports_text);
    
    QPushButton* close_button = new QPushButton("Close");
    connect(close_button, &QPushButton::clicked, imports_dialog, &QDialog::close);
    layout->addWidget(close_button);
    
    imports_dialog->show();
    
    log_message(QString("Imports analysis completed - %1 imports found").arg(imports.size()));
}

void MainWindow::on_action_show_exports_triggered() {
    if (!elf_parser->is_valid_elf()) {
        QMessageBox::warning(this, "No File", "Please load a binary file first.");
        return;
    }
    
    log_message("Analyzing exports...");
    
    std::vector<Export> exports = elf_parser->get_exports();
    
    QString exports_content = "=== EXPORTS ANALYSIS ===\n\n";
    exports_content += QString("Total exports found: %1\n\n").arg(exports.size());
    
    if (exports.empty()) {
        exports_content += "No exports found in this binary.\n";
        exports_content += "This might be an executable rather than a shared library.\n";
    } else {
        exports_content += "Function\t\t\t\tAddress\t\tType\n";
        exports_content += "================================================================\n";
        
        for (const auto& export_item : exports) {
            exports_content += QString("%1\t\t\t\t0x%2\t\t%3\n")
                             .arg(QString::fromStdString(export_item.name), -30)
                             .arg(export_item.address, 8, 16, QChar('0'))
                             .arg(QString::fromStdString(export_item.type));
        }
    }
    
    // Create a new dialog to show exports
    QDialog* exports_dialog = new QDialog(this);
    exports_dialog->setWindowTitle("Exports Analysis");
    exports_dialog->setModal(false);
    exports_dialog->resize(800, 600);
    
    QVBoxLayout* layout = new QVBoxLayout(exports_dialog);
    QTextEdit* exports_text = new QTextEdit();
    exports_text->setPlainText(exports_content);
    exports_text->setReadOnly(true);
    exports_text->setFont(QFont("Consolas", 10));
    
    layout->addWidget(exports_text);
    
    QPushButton* close_button = new QPushButton("Close");
    connect(close_button, &QPushButton::clicked, exports_dialog, &QDialog::close);
    layout->addWidget(close_button);
    
    exports_dialog->show();
    
    log_message(QString("Exports analysis completed - %1 exports found").arg(exports.size()));
}

void MainWindow::update_debug_state() {
    // Get current debugger state
    DebuggerState state = debugger_engine->get_state();
    current_debug_state = state;
    
    // Update status bar
    switch (state) {
        case DebuggerState::NOT_RUNNING:
            debug_state_label->setText("Not Running");
            break;
        case DebuggerState::RUNNING:
            debug_state_label->setText("Running");
            break;
        case DebuggerState::PAUSED:
            debug_state_label->setText("Paused");
            break;
        case DebuggerState::STOPPED:
            debug_state_label->setText("Stopped");
            break;
        case DebuggerState::ERROR:
            debug_state_label->setText("Error");
            break;
    }
    
    // Update debug controls
    update_debug_controls();
    
    // If paused, update views
    if (state == DebuggerState::PAUSED) {
        refresh_views();
        
        // Update current instruction pointer
        current_address = debugger_engine->get_instruction_pointer();
        highlight_current_instruction(current_address);
        
        log_message(QString("Debug state updated - current address: 0x%1").arg(current_address, 0, 16));
    }
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
    // Only refresh if we're in a debug session
    if (current_debug_state != DebuggerState::PAUSED) {
        return;
    }
    
    try {
        // Update registers view
        std::vector<Register> registers = debugger_engine->get_registers();
        registers_view->set_registers(registers);
        
        // Update memory view - show memory around current instruction pointer
        uint64_t ip = debugger_engine->get_instruction_pointer();
        if (ip != 0) {
            // Read 512 bytes around the current instruction pointer
            uint64_t start_addr = (ip >= 256) ? ip - 256 : 0;
            std::vector<uint8_t> memory = debugger_engine->read_memory(start_addr, 512);
            if (!memory.empty()) {
                memory_view->set_memory_data(start_addr, memory);
            }
        }
        
        // Update breakpoints view
        std::vector<Breakpoint> breakpoints = debugger_engine->get_breakpoints();
        breakpoint_view->set_breakpoints(breakpoints);
        
        log_message("Views refreshed");
        
    } catch (const std::exception& e) {
        log_message(QString("Error refreshing views: %1").arg(e.what()));
    } catch (...) {
        log_message("Unknown error occurred while refreshing views");
    }
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