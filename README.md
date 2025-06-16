# Advanced Debugger

A comprehensive GUI-based debugger with disassembly, decompilation, and debugging capabilities for Linux systems.

## Features

### 🔧 Core Capabilities
- **Multi-Architecture Disassembler**: Support for x86, x86-64, and ARM architectures
- **Intelligent Decompiler**: Convert assembly code back to readable C-like pseudocode
- **Advanced Debugging**: Set breakpoints, step through code, and inspect memory/registers
- **ELF Binary Analysis**: Parse and analyze ELF executables and shared libraries
- **Symbol Table Management**: Automatic symbol resolution and function identification

### 🖥️ GUI Features
- **Disassembly View**: Interactive assembly code viewer with syntax highlighting
- **Decompiler View**: Side-by-side assembly and decompiled C code
- **Memory Browser**: Hexadecimal memory viewer with annotations
- **Register Inspector**: Real-time CPU register monitoring
- **Breakpoint Manager**: Visual breakpoint management and control
- **Project Management**: Save and load debugging sessions

### 🛠️ Technical Features
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

## Architecture

```
debugger/
├── src/
│   ├── main.cpp                    # Application entry point
│   ├── core/                       # Core functionality
│   │   ├── project.cpp             # Project management
│   │   ├── symbol_table.cpp        # Symbol resolution
│   │   └── utils.cpp               # Utility functions
│   ├── disassembler/               # Disassembly engine
│   │   ├── disassembler.cpp        # Main disassembler
│   │   ├── elf_parser.cpp          # ELF file parsing
│   │   └── architecture.cpp        # Architecture-specific code
│   ├── decompiler/                 # Decompilation engine
│   │   ├── decompiler.cpp          # Main decompiler
│   │   ├── pattern_matcher.cpp     # Code pattern recognition
│   │   └── ast_builder.cpp         # Abstract Syntax Tree builder
│   ├── debugger/                   # Debug engine
│   │   ├── debugger.cpp            # Main debugger controller
│   │   ├── breakpoint.cpp          # Breakpoint management
│   │   ├── memory_manager.cpp      # Memory operations
│   │   └── process_control.cpp     # Process control via ptrace
│   └── gui/                        # GUI components
│       ├── main_window.cpp         # Main application window
│       ├── disassembly_view.cpp    # Assembly code viewer
│       ├── decompiler_view.cpp     # Decompiled code viewer
│       ├── debugger_view.cpp       # Debug control panel
│       ├── memory_view.cpp         # Memory browser
│       └── registers_view.cpp      # Register inspector
├── include/                        # Header files
└── build/                          # Build directory
```

## Dependencies

### Required Libraries
- **Qt5**: GUI framework
- **Capstone Engine**: Multi-architecture disassembler
- **ELFIO**: ELF file parsing library
- **CMake**: Build system (3.16+)
- **GCC/Clang**: C++17 compatible compiler

### Ubuntu/Debian Installation
```bash
sudo apt update
sudo apt install -y \
    build-essential \
    cmake \
    qt5-default \
    qttools5-dev-tools \
    libcapstone-dev \
    libelfio-dev \
    pkg-config
```

### Fedora/RHEL Installation
```bash
sudo dnf install -y \
    gcc-c++ \
    cmake \
    qt5-qtbase-devel \
    qt5-qttools-devel \
    capstone-devel \
    elfio-devel \
    pkgconfig
```

### Arch Linux Installation
```bash
sudo pacman -S \
    base-devel \
    cmake \
    qt5-base \
    qt5-tools \
    capstone \
    elfio
```

## Building

### Quick Build
```bash
# Clone and navigate to project directory
cd /path/to/debugger

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build the project
make -j$(nproc)

# Install (optional)
sudo make install
```

### Development Build with Debug Symbols
```bash
mkdir build-debug && cd build-debug
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j$(nproc)
```

### Release Build (Optimized)
```bash
mkdir build-release && cd build-release
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
```

## Usage

### Basic Usage
```bash
# Launch the debugger
./debugger

# Or debug a specific binary
./debugger /path/to/binary
```

### Command Line Options
```bash
./debugger [OPTIONS] [BINARY]

Options:
  -h, --help              Show help message
  -v, --version           Show version information
  -a, --arch ARCH         Force architecture (x86, x64, arm)
  --no-gui                Run in command-line mode
  --symbols FILE          Load external symbol file
```

### Keyboard Shortcuts
- **F5**: Start/Continue debugging
- **F9**: Toggle breakpoint at current line
- **F10**: Step over
- **F11**: Step into
- **Shift+F11**: Step out
- **Ctrl+G**: Go to address
- **Ctrl+F**: Find/Search
- **Ctrl+S**: Save project

## Features in Detail

### Disassembler
- Support for x86, x86-64, and ARM instruction sets
- Intelligent function detection and boundary analysis
- Cross-reference generation for jumps and calls
- Import/export table analysis
- String and constant identification

### Decompiler
- Pattern-based decompilation to C-like pseudocode
- Control flow reconstruction (if/else, loops, switch)
- Variable type inference
- Function signature reconstruction
- Comment generation for unclear operations

### Debugger
- Full process control (start, stop, step, continue)
- Hardware and software breakpoints
- Memory and register inspection/modification
- Stack trace analysis
- Thread management
- Conditional breakpoints

## Development

### Adding New Architectures
1. Extend `Architecture` class in `src/disassembler/architecture.cpp`
2. Add architecture-specific patterns in `src/decompiler/pattern_matcher.cpp`
3. Update CMakeLists.txt if new dependencies are required

### Contributing
1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

### Coding Standards
- Follow C++17 standards
- Use Qt naming conventions for GUI code
- Document public APIs with Doxygen comments
- Include unit tests for new functionality

## Troubleshooting

### Common Issues

**Capstone not found**
```bash
# Ensure Capstone is installed
sudo apt install libcapstone-dev  # Ubuntu/Debian
sudo dnf install capstone-devel   # Fedora
```

**Qt5 not found**
```bash
# Install Qt5 development packages
sudo apt install qt5-default qttools5-dev-tools
```

**Permission denied during debugging**
```bash
# Add user to required groups or run with elevated privileges
sudo setcap cap_sys_ptrace=eip ./debugger
```

### Debug Mode
Enable verbose logging by setting environment variables:
```bash
export DEBUGGER_LOG_LEVEL=DEBUG
export DEBUGGER_LOG_FILE=debugger.log
./debugger
```

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- **Capstone Engine**: For providing excellent disassembly capabilities
- **Qt Framework**: For the robust GUI framework
- **ELFIO**: For ELF file parsing functionality
- **Linux Community**: For ptrace API documentation and examples

## Roadmap

### Version 1.1 (Upcoming)
- [ ] Plugin system for custom analysis modules
- [ ] Scripting interface (Python bindings)
- [ ] Advanced anti-debugging detection
- [ ] Remote debugging capabilities

### Version 1.2 (Future)
- [ ] Windows PE file support
- [ ] MacOS Mach-O file support
- [ ] RISC-V architecture support
- [ ] Machine learning-based decompilation improvements

---

For more information, documentation, and updates, visit the project repository. 