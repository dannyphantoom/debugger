cmake_minimum_required(VERSION 3.16)
project(AdvancedDebugger VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Core Widgets)

# Enable Qt5 automatic MOC generation
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Capstone disassembly library
pkg_check_modules(CAPSTONE REQUIRED capstone)

# Find ELFIO library for ELF parsing (commented out for now)
# find_path(ELFIO_INCLUDE_DIR elfio/elfio.hpp)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
# include_directories(${ELFIO_INCLUDE_DIR})

# Header files that need MOC processing
set(HEADER_FILES
    include/main_window.h
    include/disassembler.h
    include/decompiler.h
    include/debugger_engine.h
    include/elf_parser.h
)

# Source files
set(DISASSEMBLER_SOURCES
    src/disassembler/disassembler.cpp
    src/disassembler/elf_parser.cpp
    src/disassembler/architecture.cpp
)

set(DECOMPILER_SOURCES
    src/decompiler/decompiler.cpp
    src/decompiler/pattern_matcher.cpp
    src/decompiler/ast_builder.cpp
)

set(DEBUGGER_SOURCES
    src/debugger/debugger.cpp
    src/debugger/breakpoint.cpp
    src/debugger/memory_manager.cpp
    src/debugger/process_control.cpp
)

set(GUI_SOURCES
    src/gui/main_window.cpp
    src/gui/disassembly_view.cpp
    src/gui/decompiler_view.cpp
    src/gui/debugger_view.cpp
    src/gui/memory_view.cpp
    src/gui/registers_view.cpp
)

set(CORE_SOURCES
    src/core/project.cpp
    src/core/symbol_table.cpp
    src/core/utils.cpp
)

# Main executable
add_executable(debugger
    src/main.cpp
    ${HEADER_FILES}
    ${DISASSEMBLER_SOURCES}
    ${DECOMPILER_SOURCES}
    ${DEBUGGER_SOURCES}
    ${GUI_SOURCES}
    ${CORE_SOURCES}
)

# Link libraries
target_link_libraries(debugger
    Qt5::Core
    Qt5::Widgets
    ${CAPSTONE_LIBRARIES}
    pthread
)

# Compiler flags
target_compile_options(debugger PRIVATE ${CAPSTONE_CFLAGS_OTHER})

# Install target
install(TARGETS debugger DESTINATION bin) 