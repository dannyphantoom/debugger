#include "main_window.h"
#include <QtWidgets/QApplication>
#include <QtCore/QCommandLineParser>
#include <QtCore/QDir>
#include <QtCore/QStandardPaths>
#include <QtWidgets/QMessageBox>
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Set application properties
    app.setApplicationName("Advanced Debugger");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("Debugger Project");
    app.setOrganizationDomain("debugger.local");
    
    // Setup command line parser
    QCommandLineParser parser;
    parser.setApplicationDescription("Advanced Debugger - Multi-architecture disassembler, decompiler, and debugger");
    parser.addHelpOption();
    parser.addVersionOption();
    
    // Add command line options
    QCommandLineOption archOption(QStringList() << "a" << "arch",
                                 "Force architecture <arch> (x86, x64, arm, arm64)",
                                 "arch");
    parser.addOption(archOption);
    
    QCommandLineOption noGuiOption(QStringList() << "no-gui",
                                  "Run in command-line mode (not implemented yet)");
    parser.addOption(noGuiOption);
    
    QCommandLineOption symbolsOption(QStringList() << "symbols",
                                   "Load external symbol file <file>",
                                   "file");
    parser.addOption(symbolsOption);
    
    // Add positional argument for binary file
    parser.addPositionalArgument("binary", "Binary file to analyze/debug", "[binary]");
    
    // Process command line
    parser.process(app);
    
    const QStringList args = parser.positionalArguments();
    
    // Check for no-gui option (not implemented yet)
    if (parser.isSet(noGuiOption)) {
        std::cerr << "Command-line mode not implemented yet. Use GUI mode." << std::endl;
        return 1;
    }
    
    try {
        // Create and show main window
        debugger::MainWindow window;
        window.show();
        
        // If a binary file was specified, try to open it
        if (!args.isEmpty()) {
            QString filename = args.first();
            if (QFile::exists(filename)) {
                if (!window.open_file(filename)) {
                    QMessageBox::warning(&window, "Error", 
                                       QString("Failed to open file: %1").arg(filename));
                }
            } else {
                QMessageBox::warning(&window, "Error", 
                                   QString("File not found: %1").arg(filename));
            }
        }
        
        return app.exec();
        
    } catch (const std::exception& e) {
        QMessageBox::critical(nullptr, "Fatal Error", 
                            QString("An error occurred: %1").arg(e.what()));
        return 1;
    } catch (...) {
        QMessageBox::critical(nullptr, "Fatal Error", 
                            "An unknown error occurred");
        return 1;
    }
} 