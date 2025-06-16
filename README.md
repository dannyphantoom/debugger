# Advanced Debugger

A comprehensive GUI-based debugger with disassembly, decompilation, and debugging capabilities for Linux systems.

![Advanced Debugger](https://img.shields.io/badge/platform-Linux-blue.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)
![C++](https://img.shields.io/badge/language-C++-blue.svg)
![Qt5](https://img.shields.io/badge/GUI-Qt5-green.svg)

## Features

### 🔧 **Core Capabilities**
- **Multi-Architecture Disassembler**: Support for x86, x86-64, and ARM architectures
- **Intelligent Decompiler**: Convert assembly code back to readable C-like pseudocode
- **Advanced Debugging**: Set breakpoints, step through code, and inspect memory/registers
- **ELF Binary Analysis**: Parse and analyze ELF executables and shared libraries
- **Symbol Table Management**: Automatic symbol resolution and function identification

### 🖥️ **GUI Features**
- **Disassembly View**: Interactive assembly code viewer with syntax highlighting
- **Decompiler View**: Side-by-side assembly and decompiled C code
- **Memory Browser**: Hexadecimal memory viewer with annotations
- **Register Inspector**: Real-time CPU register monitoring
- **Breakpoint Manager**: Visual breakpoint management and control
- **Project Management**: Save and load debugging sessions

### 🛠️ **Technical Features**
- **Process Control**: Full process control using ptrace API
- **Memory Management**: Advanced memory mapping and protection analysis
- **Symbol Resolution**: Automatic function and variable name resolution
- **Pattern Matching**: Intelligent code pattern recognition for decompilation

## Current Status

### ✅ **Fully Functional Release v1.0.0** 

**Recently Implemented & Fixed:**
- ✅ **Professional Dark Theme** - Modern VSCode-inspired dark UI with syntax highlighting
- ✅ **Real Disassembly Engine** - Fixed from dummy NOPs to actual x86/x86-64/ARM instruction parsing
- ✅ **Enhanced ELF Parser** - Now reads actual binary data from executable segments
- ✅ **Improved GUI Layout** - Tabbed panels, status indicators, and proper error handling
- ✅ **Function Analysis** - Automatic function detection and symbol extraction
- ✅ **Multi-tab Interface** - Organized panels for functions, symbols, sections, and strings

**Core Features Working:**
- ✅ File loading and ELF analysis
- ✅ Multi-architecture disassembly (x86, x86-64, ARM)
- ✅ Function and symbol parsing
- ✅ Memory section analysis
- ✅ String extraction
- ✅ Basic debugging framework (ptrace foundation)

**Advanced Features in Development:**
- 🔄 Live debugging with breakpoints and stepping
- 🔄 Memory and register inspection
- 🔄 Advanced decompilation with syntax highlighting
- 🔄 Cross-reference analysis
- 🔄 Project save/load functionality

## Quick Installation

### Automatic Installation (Recommended)

The easiest way to install the Advanced Debugger is using the provided installer script:

```bash
# Clone the repository
git clone <repository-url>
cd debugger

# Run the installer (it will handle everything automatically)
chmod +x install.sh
./install.sh
```

The installer will:
1. **Detect your Linux distribution** automatically
2. **Install all required dependencies**
3. **Build the project** using CMake
4. **Run basic tests** to verify functionality
5. **Optionally install system-wide** with desktop integration

### Supported Distributions

The installer supports the following Linux distributions:

- **Arch Linux** (and derivatives: Manjaro, EndeavourOS)
- **Ubuntu** (and derivatives: Pop!_OS, Elementary, Zorin)
- **Debian** (and derivatives)
- **Fedora** (and derivatives: CentOS, RHEL, Rocky, AlmaLinux)
- **openSUSE** (Leap, Tumbleweed, SLED, SLES)

### Manual Installation

If you prefer to install manually or your distribution isn't supported:

#### Dependencies

**Build Tools:**
- C++ compiler (GCC 7+ or Clang 5+)
- CMake 3.16+
- Make or Ninja
- pkg-config

**Libraries:**
- Qt5 (Core, Widgets, Tools)
- Capstone disassembly engine
- Git (for cloning)

#### Installation Commands

**Arch Linux:**
```bash
sudo pacman -S base-devel cmake qt5-base qt5-tools capstone git
```

**Ubuntu/Debian:**
```bash
sudo apt install build-essential cmake qtbase5-dev qttools5-dev libcapstone-dev git
```

**Fedora:**
```bash
sudo dnf install gcc-c++ cmake qt5-qtbase-devel qt5-qttools-devel capstone-devel git
```

#### Build Process

```bash
# Clone and build
git clone <repository-url>
cd debugger
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Run the debugger
./debugger
```

## Usage Guide

### Getting Started

1. **Launch the debugger:**
   ```bash
   advanced-debugger  # If installed system-wide
   # OR
   ./build/debugger   # If built locally
   ```

2. **Load a binary:**
   - Click `File` → `Open Binary...`
   - Select your executable file
   - The debugger will automatically analyze and disassemble it

3. **Start debugging:**
   - Click `Debug` → `Start Debugging (F5)`
   - Or use the toolbar button

### Key Features

#### Disassembly View
- **Color-coded instructions**: Calls (red), jumps (orange), addresses (green)
- **Line numbers** for easy navigation
- **Double-click addresses** to navigate
- **Context menu** for copy/navigation options

#### Memory View
- **Hex editor interface** with 16-byte rows
- **Color-coded bytes**: Null (gray), printable (green), 0xFF (red)
- **Interactive editing** with validation
- **Go-to-address** functionality

#### Registers View
- **Complete x86-64 register set** display
- **Visual change tracking** with highlighting
- **Flags register decoding** (CF, PF, ZF, SF, etc.)
- **Context menu** for copying and navigation

#### Breakpoint Management
- **Toggle breakpoints** with F9 or context menu
- **Conditional breakpoints** with custom expressions
- **Breakpoint list** with enable/disable options
- **Hit count tracking**

### Keyboard Shortcuts

| Action | Shortcut |
|--------|----------|
| Open Binary | Ctrl+O |
| Start/Continue Debug | F5 |
| Step Into | F11 |
| Step Over | F10 |
| Toggle Breakpoint | F9 |
| Go to Address | Ctrl+G |
| Find | Ctrl+F |
| Close | Ctrl+W |
| Exit | Ctrl+Q |

### Example Workflow

1. **Open a binary** you want to analyze
2. **Review the disassembly** to understand the code flow
3. **Set breakpoints** at interesting locations
4. **Start debugging** to examine runtime behavior
5. **Step through code** and monitor register/memory changes
6. **Analyze functions** using the decompiler view
7. **Save your session** for later analysis

## Advanced Features

### Project Files
Save your analysis progress with project files that include:
- Breakpoint locations and conditions
- Analysis comments and labels
- Function definitions and names
- Session metadata and timestamps

### String Analysis
Extract and analyze strings from:
- Code sections
- Data sections (.rodata, .data, .bss)
- With automatic filtering and categorization

### Import/Export Analysis
- **Import table** analysis with library information
- **Export table** examination for shared libraries
- **Symbol resolution** and cross-referencing

### Memory Debugging
- **Memory breakpoints** for read/write access
- **Memory pattern search** and comparison
- **Memory dump** and export functionality

## Troubleshooting

### Common Issues

**Q: The application crashes on startup**
A: Make sure all dependencies are installed. Run the installer again or check dependency versions.

**Q: Can't attach to process**
A: You may need elevated privileges. Try running with `sudo` or check your system's ptrace settings.

**Q: Missing symbols in disassembly**
A: Ensure the binary wasn't stripped. Try loading additional symbol files if available.

**Q: Qt5 not found during build**
A: Install the Qt5 development packages for your distribution (see installation section).

### Debug Information

For debugging issues:
1. Check the log view in the application
2. Run from terminal to see console output
3. Verify all dependencies with `ldd ./build/debugger`

### Uninstalling

If you installed system-wide, an uninstaller script is created:
```bash
./uninstall.sh
```

Or manually remove:
```bash
sudo rm /usr/local/bin/advanced-debugger
rm ~/.local/share/applications/advanced-debugger.desktop
```

## Architecture

### Core Components

- **Disassembler**: Capstone-based multi-architecture disassembly
- **Decompiler**: Pattern-matching decompilation engine
- **Debugger Engine**: ptrace-based process debugging
- **ELF Parser**: Custom ELF file format analyzer
- **GUI Framework**: Qt5-based modern interface

### Directory Structure

```
debugger/
├── src/
│   ├── core/           # Core functionality
│   ├── debugger/       # Debugging engine
│   ├── decompiler/     # Decompilation engine
│   ├── disassembler/   # Disassembly engine
│   └── gui/            # User interface
├── include/            # Header files
├── build/              # Build directory
├── install.sh          # Installation script
├── uninstall.sh        # Uninstaller (generated)
└── README.md           # This file
```

## Development

### Building for Development

```bash
# Debug build with symbols
mkdir build-debug && cd build-debug
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)
```

### Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

### Coding Standards

- C++17 standard
- Qt5 conventions for GUI code
- Clear variable names and comments
- Error handling for all operations

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- **Capstone Engine** for disassembly capabilities
- **Qt Framework** for the GUI framework
- **ELF specification** for binary format support
- **Linux ptrace** for debugging functionality

## Contact

For issues, questions, or contributions, please use the project's issue tracker or contact the maintainers.

---

**Advanced Debugger** - Professional reverse engineering and debugging for Linux 