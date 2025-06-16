#pragma once

#include "disassembler.h"
#include <memory>
#include <string>
#include <vector>
#include <map>
// #include <elfio/elfio.hpp>  // Commented out for now

namespace debugger {

struct Symbol {
    std::string name;
    uint64_t address;
    uint64_t size;
    std::string type;
    std::string binding;
    std::string section_name;
    bool is_function;
    bool is_imported;
    bool is_exported;
};

struct Section {
    std::string name;
    uint64_t address;
    uint64_t size;
    uint64_t file_offset;
    std::string type;
    std::string flags;
    std::vector<uint8_t> data;
    bool is_executable;
    bool is_writable;
    bool is_readable;
};

struct Import {
    std::string name;
    std::string library;
    uint64_t address;
    std::string type;
};

struct Export {
    std::string name;
    uint64_t address;
    std::string type;
};

struct ElfInfo {
    std::string filename;
    Architecture architecture;
    std::string entry_point_hex;
    uint64_t entry_point;
    std::string file_type;
    bool is_64bit;
    bool is_little_endian;
    std::string machine_type;
    std::vector<Section> sections;
    std::vector<Symbol> symbols;
    std::vector<Import> imports;
    std::vector<Export> exports;
    std::map<std::string, std::string> metadata;
};

class ElfParser {
public:
    ElfParser();
    ~ElfParser() = default;

    // Main parsing functions
    bool load_file(const std::string& filename);
    ElfInfo get_elf_info() const;
    
    // Section operations
    std::vector<Section> get_sections() const;
    Section get_section(const std::string& name) const;
    std::vector<uint8_t> get_section_data(const std::string& name) const;
    bool has_section(const std::string& name) const;
    
    // Symbol operations
    std::vector<Symbol> get_symbols() const;
    std::vector<Symbol> get_functions() const;
    Symbol find_symbol(const std::string& name) const;
    Symbol find_symbol_by_address(uint64_t address) const;
    std::string get_function_name(uint64_t address) const;
    
    // Import/Export operations
    std::vector<Import> get_imports() const;
    std::vector<Export> get_exports() const;
    
    // Memory mapping
    uint64_t virtual_to_file_offset(uint64_t virtual_address) const;
    uint64_t file_to_virtual_offset(uint64_t file_offset) const;
    
    // Architecture detection
    Architecture detect_architecture() const;
    bool is_64bit() const;
    bool is_little_endian() const;
    
    // Utility functions
    bool is_valid_elf() const;
    std::string get_last_error() const;
    std::vector<uint8_t> get_code_section_data() const;
    uint64_t get_entry_point() const;
    
    // Static analysis helpers
    std::vector<uint64_t> find_string_references(const std::string& str) const;
    std::vector<uint64_t> find_function_calls(uint64_t target_address) const;
    std::map<uint64_t, std::string> build_address_to_symbol_map() const;

private:
    // ELFIO::elfio elf_reader;  // Commented out for now
    std::string filename;
    std::string last_error;
    bool loaded;
    ElfInfo cached_info;
    bool info_cached;
    
    // Helper functions
    void parse_sections();
    void parse_symbols();
    void parse_imports_exports();
    void cache_elf_info();
    void parse_program_headers();
    void read_code_segment(uint64_t file_offset, uint64_t vaddr, uint64_t size);
    std::string get_section_type_string(uint32_t type) const;
    std::string get_section_flags_string(uint64_t flags) const;
    std::string get_symbol_type_string(unsigned char type) const;
    std::string get_symbol_binding_string(unsigned char binding) const;
    Architecture elf_machine_to_architecture(uint16_t machine) const;
    bool is_section_executable(uint32_t flags) const;
    bool is_section_writable(uint32_t flags) const;
    bool is_section_readable(uint32_t flags) const;
};

} // namespace debugger 