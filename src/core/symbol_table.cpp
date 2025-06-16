#include <string>
#include <map>
#include <vector>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>

namespace debugger {

class SymbolTable {
public:
    struct SymbolInfo {
        std::string name;
        uint64_t address;
        uint64_t size;
        std::string type;
        std::string section;
        bool is_function;
        bool is_global;
    };

    bool load_symbols(const std::string& filename) {
        clear();
        
        // Try to load symbols from different formats
        if (load_elf_symbols(filename)) {
            symbol_file = filename;
            return true;
        }
        
        // Fallback to text symbol file format
        if (load_text_symbols(filename)) {
            symbol_file = filename;
            return true;
        }
        
        return false;
    }
    
    std::string lookup_symbol(uint64_t address) {
        // Direct address lookup
        auto direct_it = address_to_symbol.find(address);
        if (direct_it != address_to_symbol.end()) {
            return direct_it->second;
        }
        
        // Find closest symbol before this address
        auto it = address_to_symbol.upper_bound(address);
        if (it != address_to_symbol.begin()) {
            --it;
            
            // Check if address falls within symbol range
            auto symbol_it = symbol_info.find(it->second);
            if (symbol_it != symbol_info.end()) {
                uint64_t symbol_start = it->first;
                uint64_t symbol_end = symbol_start + symbol_it->second.size;
                
                if (address >= symbol_start && address < symbol_end) {
                    uint64_t offset = address - symbol_start;
                    if (offset == 0) {
                        return it->second;
                    } else {
                        return it->second + "+" + std::to_string(offset);
                    }
                }
            }
        }
        
        return "";
    }
    
    uint64_t lookup_address(const std::string& symbol) {
        auto it = symbol_to_address.find(symbol);
        if (it != symbol_to_address.end()) {
            return it->second;
        }
        
        // Try partial matching
        for (const auto& pair : symbol_to_address) {
            if (pair.first.find(symbol) != std::string::npos) {
                return pair.second;
            }
        }
        
        return 0;
    }
    
    void add_symbol(const std::string& name, uint64_t address, uint64_t size = 0, 
                   const std::string& type = "", bool is_function = false) {
        address_to_symbol[address] = name;
        symbol_to_address[name] = address;
        
        SymbolInfo info;
        info.name = name;
        info.address = address;
        info.size = size;
        info.type = type;
        info.is_function = is_function;
        info.is_global = true;
        
        symbol_info[name] = info;
    }
    
    void clear() {
        address_to_symbol.clear();
        symbol_to_address.clear();
        symbol_info.clear();
        symbol_file.clear();
    }
    
    size_t size() const {
        return symbol_to_address.size();
    }
    
    // Advanced symbol operations
    std::vector<std::string> find_symbols_by_pattern(const std::string& pattern) {
        std::vector<std::string> matches;
        
        for (const auto& pair : symbol_to_address) {
            if (pair.first.find(pattern) != std::string::npos) {
                matches.push_back(pair.first);
            }
        }
        
        return matches;
    }
    
    std::vector<std::string> get_function_symbols() {
        std::vector<std::string> functions;
        
        for (const auto& pair : symbol_info) {
            if (pair.second.is_function) {
                functions.push_back(pair.first);
            }
        }
        
        return functions;
    }
    
    std::vector<SymbolInfo> get_symbols_in_range(uint64_t start_addr, uint64_t end_addr) {
        std::vector<SymbolInfo> symbols_in_range;
        
        auto start_it = address_to_symbol.lower_bound(start_addr);
        auto end_it = address_to_symbol.upper_bound(end_addr);
        
        for (auto it = start_it; it != end_it; ++it) {
            auto info_it = symbol_info.find(it->second);
            if (info_it != symbol_info.end()) {
                symbols_in_range.push_back(info_it->second);
            }
        }
        
        return symbols_in_range;
    }
    
    bool has_symbol(const std::string& name) {
        return symbol_to_address.find(name) != symbol_to_address.end();
    }
    
    SymbolInfo get_symbol_info(const std::string& name) {
        auto it = symbol_info.find(name);
        if (it != symbol_info.end()) {
            return it->second;
        }
        return SymbolInfo{};
    }
    
    std::string get_symbol_file() const {
        return symbol_file;
    }
    
    // Export symbols to file
    bool export_symbols(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        file << "# Symbol table exported from debugger\n";
        file << "# Format: address name type size\n";
        
        for (const auto& pair : symbol_info) {
            const SymbolInfo& info = pair.second;
            file << std::hex << "0x" << info.address << " "
                 << info.name << " "
                 << info.type << " "
                 << std::dec << info.size << "\n";
        }
        
        return true;
    }
    
private:
    std::map<uint64_t, std::string> address_to_symbol;
    std::map<std::string, uint64_t> symbol_to_address;
    std::map<std::string, SymbolInfo> symbol_info;
    std::string symbol_file;
    
    bool load_elf_symbols(const std::string& filename) {
        // This would integrate with ELF parser to extract symbols
        // For now, return false to use text format
        return false;
    }
    
    bool load_text_symbols(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        int line_number = 0;
        
        while (std::getline(file, line)) {
            line_number++;
            
            // Skip comments and empty lines
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            std::istringstream iss(line);
            std::string address_str, name, type;
            uint64_t size = 0;
            
            if (!(iss >> address_str >> name)) {
                std::cerr << "Warning: Invalid line " << line_number 
                         << " in symbol file: " << line << std::endl;
                continue;
            }
            
            // Optional type and size
            iss >> type >> size;
            
            // Parse address
            uint64_t address = 0;
            try {
                if (address_str.substr(0, 2) == "0x" || address_str.substr(0, 2) == "0X") {
                    address = std::stoull(address_str, nullptr, 16);
                } else {
                    address = std::stoull(address_str, nullptr, 10);
                }
            } catch (const std::exception& e) {
                std::cerr << "Warning: Invalid address '" << address_str 
                         << "' on line " << line_number << std::endl;
                continue;
            }
            
            // Determine if it's a function
            bool is_function = (type == "FUNC" || type == "function" || 
                              name.find("_func") != std::string::npos ||
                              name.find("sub_") == 0);
            
            add_symbol(name, address, size, type, is_function);
        }
        
        return size() > 0;
    }
};

// Static instance for global access
static SymbolTable global_symbol_table;

// C-style interface for compatibility
extern "C" {
    bool load_symbol_table(const char* filename) {
        return global_symbol_table.load_symbols(filename);
    }
    
    const char* lookup_symbol_name(uint64_t address) {
        static std::string result;
        result = global_symbol_table.lookup_symbol(address);
        return result.empty() ? nullptr : result.c_str();
    }
    
    uint64_t lookup_symbol_address(const char* name) {
        return global_symbol_table.lookup_address(name);
    }
    
    void add_symbol_entry(const char* name, uint64_t address) {
        global_symbol_table.add_symbol(name, address);
    }
    
    void clear_symbol_table() {
        global_symbol_table.clear();
    }
    
    size_t get_symbol_count() {
        return global_symbol_table.size();
    }
}

} // namespace debugger 