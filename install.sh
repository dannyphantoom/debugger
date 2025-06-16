#!/bin/bash

# Advanced Debugger Installer Script
# Supports multiple Linux distributions and handles all dependencies

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Print colored output
print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if running as root
check_root() {
    if [[ $EUID -eq 0 ]]; then
        print_error "This script should not be run as root for security reasons."
        print_info "Please run as a regular user. The script will ask for sudo when needed."
        exit 1
    fi
}

# Detect Linux distribution
detect_distro() {
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        DISTRO=$ID
        VERSION=$VERSION_ID
    elif type lsb_release >/dev/null 2>&1; then
        DISTRO=$(lsb_release -si | tr '[:upper:]' '[:lower:]')
        VERSION=$(lsb_release -sr)
    else
        print_error "Cannot detect Linux distribution"
        exit 1
    fi
    
    print_info "Detected distribution: $DISTRO $VERSION"
}

# Install dependencies based on distribution
install_dependencies() {
    print_info "Installing dependencies for $DISTRO..."
    
    case $DISTRO in
        "arch"|"manjaro"|"endeavouros")
            print_info "Installing packages using pacman..."
            sudo pacman -Syu --noconfirm
            sudo pacman -S --noconfirm \
                base-devel \
                cmake \
                qt5-base \
                qt5-tools \
                capstone \
                git
            ;;
        
        "ubuntu"|"debian"|"pop"|"elementary"|"zorin")
            print_info "Installing packages using apt..."
            sudo apt update
            sudo apt install -y \
                build-essential \
                cmake \
                qtbase5-dev \
                qttools5-dev \
                qttools5-dev-tools \
                libcapstone-dev \
                libcapstone4 \
                pkg-config \
                git
            ;;
        
        "fedora"|"centos"|"rhel"|"rocky"|"almalinux")
            print_info "Installing packages using dnf/yum..."
            if command -v dnf &> /dev/null; then
                PKG_MGR="dnf"
            else
                PKG_MGR="yum"
            fi
            
            sudo $PKG_MGR update -y
            sudo $PKG_MGR install -y \
                gcc-c++ \
                make \
                cmake \
                qt5-qtbase-devel \
                qt5-qttools-devel \
                capstone-devel \
                pkgconfig \
                git
            ;;
        
        "opensuse"|"opensuse-leap"|"opensuse-tumbleweed"|"sled"|"sles")
            print_info "Installing packages using zypper..."
            sudo zypper refresh
            sudo zypper install -y \
                gcc-c++ \
                make \
                cmake \
                libqt5-qtbase-devel \
                libqt5-qttools-devel \
                libcapstone-devel \
                pkg-config \
                git
            ;;
        
        *)
            print_error "Unsupported distribution: $DISTRO"
            print_info "Please install the following packages manually:"
            print_info "- C++ compiler (gcc/g++ or clang)"
            print_info "- CMake (version 3.16 or higher)"
            print_info "- Qt5 development libraries (qtbase5-dev, qttools5-dev)"
            print_info "- Capstone disassembly library (libcapstone-dev)"
            print_info "- pkg-config"
            print_info "- git"
            exit 1
            ;;
    esac
    
    print_success "Dependencies installed successfully!"
}

# Check if dependencies are available
check_dependencies() {
    print_info "Checking if all dependencies are available..."
    
    local missing_deps=()
    
    # Check for C++ compiler
    if ! command -v g++ &> /dev/null && ! command -v clang++ &> /dev/null; then
        missing_deps+=("C++ compiler (g++ or clang++)")
    fi
    
    # Check for CMake
    if ! command -v cmake &> /dev/null; then
        missing_deps+=("cmake")
    else
        CMAKE_VERSION=$(cmake --version | head -n1 | cut -d' ' -f3)
        print_info "Found CMake version: $CMAKE_VERSION"
    fi
    
    # Check for pkg-config
    if ! command -v pkg-config &> /dev/null; then
        missing_deps+=("pkg-config")
    fi
    
    # Check for Qt5
    if ! pkg-config --exists Qt5Core; then
        missing_deps+=("Qt5 development libraries")
    else
        QT5_VERSION=$(pkg-config --modversion Qt5Core)
        print_info "Found Qt5 version: $QT5_VERSION"
    fi
    
    # Check for Capstone
    if ! pkg-config --exists capstone; then
        missing_deps+=("Capstone library")
    else
        CAPSTONE_VERSION=$(pkg-config --modversion capstone)
        print_info "Found Capstone version: $CAPSTONE_VERSION"
    fi
    
    if [ ${#missing_deps[@]} -ne 0 ]; then
        print_error "Missing dependencies:"
        for dep in "${missing_deps[@]}"; do
            print_error "  - $dep"
        done
        return 1
    fi
    
    print_success "All dependencies are available!"
    return 0
}

# Build the project
build_project() {
    print_info "Building the Advanced Debugger..."
    
    local build_dir="build"
    
    # Clean previous build if it exists
    if [ -d "$build_dir" ]; then
        print_info "Cleaning previous build directory..."
        rm -rf "$build_dir"
    fi
    
    # Create build directory
    mkdir -p "$build_dir"
    cd "$build_dir"
    
    # Configure with CMake
    print_info "Configuring build with CMake..."
    cmake .. -DCMAKE_BUILD_TYPE=Release
    
    # Build the project
    print_info "Compiling the project..."
    make -j$(nproc)
    
    cd ..
    
    print_success "Build completed successfully!"
}

# Install the debugger
install_debugger() {
    print_info "Installing the Advanced Debugger..."
    
    local install_prefix="/usr/local"
    local bin_dir="$install_prefix/bin"
    local desktop_dir="$HOME/.local/share/applications"
    
    # Install binary
    print_info "Installing binary to $bin_dir..."
    sudo install -m 755 build/debugger "$bin_dir/advanced-debugger"
    
    # Create desktop entry
    print_info "Creating desktop entry..."
    mkdir -p "$desktop_dir"
    
    cat > "$desktop_dir/advanced-debugger.desktop" << EOF
[Desktop Entry]
Name=Advanced Debugger
Comment=Comprehensive GUI-based debugger with disassembly and decompilation
Exec=advanced-debugger
Icon=application-x-executable
Terminal=false
Type=Application
Categories=Development;Debugger;
Keywords=debugger;disassembler;decompiler;reverse engineering;
EOF
    
    # Update desktop database if available
    if command -v update-desktop-database &> /dev/null; then
        update-desktop-database "$desktop_dir" 2>/dev/null || true
    fi
    
    print_success "Advanced Debugger installed successfully!"
    print_info "You can now run it with: advanced-debugger"
    print_info "Or find it in your application menu under Development"
}

# Create uninstaller script
create_uninstaller() {
    print_info "Creating uninstaller script..."
    
    cat > uninstall.sh << 'EOF'
#!/bin/bash

# Advanced Debugger Uninstaller

print_info() {
    echo -e "\033[0;34m[INFO]\033[0m $1"
}

print_success() {
    echo -e "\033[0;32m[SUCCESS]\033[0m $1"
}

print_info "Uninstalling Advanced Debugger..."

# Remove binary
if [ -f "/usr/local/bin/advanced-debugger" ]; then
    sudo rm -f /usr/local/bin/advanced-debugger
    print_info "Removed binary from /usr/local/bin/"
fi

# Remove desktop entry
if [ -f "$HOME/.local/share/applications/advanced-debugger.desktop" ]; then
    rm -f "$HOME/.local/share/applications/advanced-debugger.desktop"
    print_info "Removed desktop entry"
fi

# Update desktop database
if command -v update-desktop-database &> /dev/null; then
    update-desktop-database "$HOME/.local/share/applications" 2>/dev/null || true
fi

print_success "Advanced Debugger uninstalled successfully!"
EOF

    chmod +x uninstall.sh
    print_info "Created uninstall.sh - run it to remove the debugger"
}

# Run tests
run_tests() {
    print_info "Running basic functionality tests..."
    
    # Test if the binary exists and is executable
    if [ ! -x "build/debugger" ]; then
        print_error "Debugger binary not found or not executable"
        return 1
    fi
    
    # Test if the binary can show help (if it has help option)
    print_info "Testing debugger executable..."
    if build/debugger --version 2>/dev/null || build/debugger --help 2>/dev/null; then
        print_success "Debugger responds to command line arguments"
    else
        print_info "Debugger doesn't support --version or --help (this is normal)"
    fi
    
    # Check if we can run it briefly (GUI apps are hard to test in CLI)
    print_info "GUI application built successfully - manual testing required"
    print_success "Basic tests passed!"
}

# Main installation function
main() {
    echo "========================================"
    echo "  Advanced Debugger Installer"
    echo "========================================"
    echo
    
    check_root
    detect_distro
    
    # Check if dependencies are already installed
    if ! check_dependencies; then
        print_info "Some dependencies are missing. Installing them now..."
        install_dependencies
        
        # Check again after installation
        if ! check_dependencies; then
            print_error "Failed to install all dependencies. Please install them manually."
            exit 1
        fi
    else
        print_info "All dependencies are already installed."
    fi
    
    build_project
    run_tests
    
    # Ask user if they want to install system-wide
    echo
    read -p "Do you want to install the debugger system-wide? (y/N): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        install_debugger
        create_uninstaller
    else
        print_info "Debugger built successfully but not installed system-wide."
        print_info "You can run it from: ./build/debugger"
        print_info "To install later, run this script again or manually copy the binary."
    fi
    
    echo
    print_success "Installation completed successfully!"
    print_info "The Advanced Debugger includes:"
    print_info "  • Multi-architecture disassembly (x86, x86-64, ARM)"
    print_info "  • ELF binary analysis and parsing"
    print_info "  • Basic decompilation to C-like pseudocode"
    print_info "  • Process debugging with breakpoints"
    print_info "  • Memory and register inspection"
    print_info "  • Dark theme UI with professional appearance"
    echo
    
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        print_info "Run 'advanced-debugger' to start the application"
    else
        print_info "Run './build/debugger' from this directory to start the application"
    fi
}

# Run the main function
main "$@" 