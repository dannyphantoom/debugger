#include "elf_parser.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <fstream>

namespace debugger {

ElfParser::ElfParser() : loaded(false), info_cached(false) {
}

bool ElfParser::load_file(const std::string& filename) {
    this->filename = filename;
    loaded = false;
    info_cached = false;
    last_error.clear();
    
    // Basic file existence check
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        last_error = "Failed to open file: " + filename;
        return false;
    }
    
    // Read ELF header (very basic check)
    char elf_header[16];
    file.read(elf_header, 16);
    
    if (file.gcount() < 16 || 
        elf_header[0] != 0x7f || elf_header[1] != 'E' || 
        elf_header[2] != 'L' || elf_header[3] != 'F') {
        last_error = "Not a valid ELF file";
        return false;
    }
    
    loaded = true;
    cache_elf_info();
    
    return true;
}

void ElfParser::cache_elf_info() {
    if (!loaded) return;
    
    cached_info.filename = filename;
    cached_info.architecture = Architecture::X86_64; // Default assumption
    cached_info.entry_point = 0x401000; // Default entry point
    
    std::stringstream ss;
    ss << "0x" << std::hex << cached_info.entry_point;
    cached_info.entry_point_hex = ss.str();
    
    cached_info.is_64bit = true; // Default assumption
    cached_info.is_little_endian = true;
    cached_info.file_type = "Executable";
    cached_info.machine_type = "x86-64";
    
    // Create dummy sections
    Section text_section;
    text_section.name = ".text";
    text_section.address = 0x401000;
    text_section.size = 0x1000;
    text_section.file_offset = 0x1000;
    text_section.type = "PROGBITS";
    text_section.flags = "AX";
    text_section.is_executable = true;
    text_section.is_writable = false;
    text_section.is_readable = true;
    
    // Add some dummy data
    text_section.data.resize(0x1000, 0x90); // Fill with NOPs
    
    cached_info.sections.push_back(text_section);
    
    // Create dummy symbols
    Symbol main_symbol;
    main_symbol.name = "main";
    main_symbol.address = 0x401000;
    main_symbol.size = 100;
    main_symbol.type = "FUNC";
    main_symbol.binding = "GLOBAL";
    main_symbol.section_name = ".text";
    main_symbol.is_function = true;
    main_symbol.is_imported = false;
    main_symbol.is_exported = true;
    
    cached_info.symbols.push_back(main_symbol);
    
    info_cached = true;
}

ElfInfo ElfParser::get_elf_info() const {
    return cached_info;
}

std::vector<Section> ElfParser::get_sections() const {
    return cached_info.sections;
}

void ElfParser::parse_sections() {
    // Stub implementation
}

void ElfParser::parse_symbols() {
    // Stub implementation
}

void ElfParser::parse_imports_exports() {
    // Stub implementation
}

Section ElfParser::get_section(const std::string& name) const {
    for (const auto& section : cached_info.sections) {
        if (section.name == name) {
            return section;
        }
    }
    return Section{}; // Return empty section if not found
}

std::vector<uint8_t> ElfParser::get_section_data(const std::string& name) const {
    Section section = get_section(name);
    return section.data;
}

bool ElfParser::has_section(const std::string& name) const {
    for (const auto& section : cached_info.sections) {
        if (section.name == name) {
            return true;
        }
    }
    return false;
}

std::vector<Symbol> ElfParser::get_symbols() const {
    return cached_info.symbols;
}

std::vector<Symbol> ElfParser::get_functions() const {
    std::vector<Symbol> functions;
    for (const auto& symbol : cached_info.symbols) {
        if (symbol.is_function) {
            functions.push_back(symbol);
        }
    }
    return functions;
}

Symbol ElfParser::find_symbol(const std::string& name) const {
    for (const auto& symbol : cached_info.symbols) {
        if (symbol.name == name) {
            return symbol;
        }
    }
    return Symbol{}; // Return empty symbol if not found
}

Symbol ElfParser::find_symbol_by_address(uint64_t address) const {
    for (const auto& symbol : cached_info.symbols) {
        if (symbol.address <= address && address < symbol.address + symbol.size) {
            return symbol;
        }
    }
    return Symbol{}; // Return empty symbol if not found
}

std::string ElfParser::get_function_name(uint64_t address) const {
    Symbol symbol = find_symbol_by_address(address);
    if (!symbol.name.empty() && symbol.is_function) {
        return symbol.name;
    }
    return "";
}

std::vector<Import> ElfParser::get_imports() const {
    return cached_info.imports;
}

std::vector<Export> ElfParser::get_exports() const {
    return cached_info.exports;
}

Architecture ElfParser::detect_architecture() const {
    return cached_info.architecture;
}

Architecture ElfParser::elf_machine_to_architecture(uint16_t machine) const {
    // Stub - always return x86_64 for now
    return Architecture::X86_64;
}

bool ElfParser::is_64bit() const {
    return cached_info.is_64bit;
}

bool ElfParser::is_little_endian() const {
    return cached_info.is_little_endian;
}

bool ElfParser::is_valid_elf() const {
    return loaded;
}

std::string ElfParser::get_last_error() const {
    return last_error;
}

std::vector<uint8_t> ElfParser::get_code_section_data() const {
    return get_section_data(".text");
}

uint64_t ElfParser::get_entry_point() const {
    return cached_info.entry_point;
}

uint64_t ElfParser::virtual_to_file_offset(uint64_t virtual_address) const {
    // Simple stub implementation
    return virtual_address - 0x400000;
}

uint64_t ElfParser::file_to_virtual_offset(uint64_t file_offset) const {
    // Simple stub implementation
    return file_offset + 0x400000;
}

std::string ElfParser::get_section_type_string(uint32_t type) const {
    return "PROGBITS"; // Stub
}

std::string ElfParser::get_section_flags_string(uint64_t flags) const {
    return "AX"; // Stub
}

std::string ElfParser::get_symbol_type_string(unsigned char type) const {
    return "FUNC"; // Stub
}

std::string ElfParser::get_symbol_binding_string(unsigned char binding) const {
    return "GLOBAL"; // Stub
}

bool ElfParser::is_section_executable(uint32_t flags) const {
    return true; // Stub
}

bool ElfParser::is_section_writable(uint32_t flags) const {
    return false; // Stub
}

bool ElfParser::is_section_readable(uint32_t flags) const {
    return true; // Stub
}

std::map<uint64_t, std::string> ElfParser::build_address_to_symbol_map() const {
    std::map<uint64_t, std::string> address_map;
    for (const auto& symbol : cached_info.symbols) {
        if (symbol.address != 0) {
            address_map[symbol.address] = symbol.name;
        }
    }
    return address_map;
}

std::vector<uint64_t> ElfParser::find_string_references(const std::string& str) const {
    return {}; // Stub
}

std::vector<uint64_t> ElfParser::find_function_calls(uint64_t target_address) const {
    return {}; // Stub
}

} // namespace debugger 