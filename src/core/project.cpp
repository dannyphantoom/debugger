#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>
#include <ctime>
#include <iomanip>
#include <algorithm>

namespace debugger {

class Project {
public:
    struct ProjectInfo {
        std::string name;
        std::string description;
        std::string binary_path;
        std::string symbol_file;
        std::string created_date;
        std::string last_modified;
        std::string version;
    };
    
    struct BreakpointInfo {
        uint64_t address;
        std::string type;
        bool enabled;
        std::string condition;
        std::string name;
    };
    
    struct AnalysisData {
        std::vector<std::string> analyzed_functions;
        std::map<uint64_t, std::string> comments;
        std::map<uint64_t, std::string> labels;
        std::vector<std::pair<uint64_t, uint64_t>> defined_functions;
    };

private:
    ProjectInfo project_info;
    std::vector<BreakpointInfo> breakpoints;
    AnalysisData analysis_data;
    std::string project_file_path;
    bool loaded;
    bool modified;

public:
    Project() : loaded(false), modified(false) {
        project_info.version = "1.0";
    }
    
    bool load(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        clear();
        project_file_path = filename;
        
        std::string line;
        std::string current_section;
        
        while (std::getline(file, line)) {
            // Remove leading/trailing whitespace
            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);
            
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            // Check for section headers
            if (line[0] == '[' && line.back() == ']') {
                current_section = line.substr(1, line.length() - 2);
                continue;
            }
            
            // Parse key-value pairs
            size_t equals_pos = line.find('=');
            if (equals_pos == std::string::npos) {
                continue;
            }
            
            std::string key = line.substr(0, equals_pos);
            std::string value = line.substr(equals_pos + 1);
            
            // Remove quotes if present
            if (value.length() >= 2 && value[0] == '"' && value.back() == '"') {
                value = value.substr(1, value.length() - 2);
            }
            
            parse_project_data(current_section, key, value);
        }
        
        loaded = true;
        modified = false;
        
        // Update last modified time
        project_info.last_modified = get_current_timestamp();
        
        return true;
    }
    
    bool save(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        project_file_path = filename;
        
        // Update timestamps
        if (project_info.created_date.empty()) {
            project_info.created_date = get_current_timestamp();
        }
        project_info.last_modified = get_current_timestamp();
        
        // Write project header
        file << "# Debugger Project File\n";
        file << "# Generated: " << project_info.last_modified << "\n\n";
        
        // Project information section
        file << "[project]\n";
        file << "name=\"" << project_info.name << "\"\n";
        file << "description=\"" << project_info.description << "\"\n";
        file << "binary_path=\"" << project_info.binary_path << "\"\n";
        file << "symbol_file=\"" << project_info.symbol_file << "\"\n";
        file << "created_date=\"" << project_info.created_date << "\"\n";
        file << "last_modified=\"" << project_info.last_modified << "\"\n";
        file << "version=\"" << project_info.version << "\"\n\n";
        
        // Breakpoints section
        if (!breakpoints.empty()) {
            file << "[breakpoints]\n";
            for (size_t i = 0; i < breakpoints.size(); ++i) {
                const auto& bp = breakpoints[i];
                file << "bp" << i << "_address=0x" << std::hex << bp.address << std::dec << "\n";
                file << "bp" << i << "_type=\"" << bp.type << "\"\n";
                file << "bp" << i << "_enabled=" << (bp.enabled ? "true" : "false") << "\n";
                if (!bp.condition.empty()) {
                    file << "bp" << i << "_condition=\"" << bp.condition << "\"\n";
                }
                if (!bp.name.empty()) {
                    file << "bp" << i << "_name=\"" << bp.name << "\"\n";
                }
            }
            file << "\n";
        }
        
        // Analysis data section
        file << "[analysis]\n";
        
        // Functions
        if (!analysis_data.analyzed_functions.empty()) {
            file << "analyzed_functions=";
            for (size_t i = 0; i < analysis_data.analyzed_functions.size(); ++i) {
                if (i > 0) file << ",";
                file << analysis_data.analyzed_functions[i];
            }
            file << "\n";
        }
        
        // Comments
        if (!analysis_data.comments.empty()) {
            for (const auto& comment : analysis_data.comments) {
                file << "comment_0x" << std::hex << comment.first << std::dec 
                     << "=\"" << comment.second << "\"\n";
            }
        }
        
        // Labels
        if (!analysis_data.labels.empty()) {
            for (const auto& label : analysis_data.labels) {
                file << "label_0x" << std::hex << label.first << std::dec 
                     << "=\"" << label.second << "\"\n";
            }
        }
        
        // Defined functions
        if (!analysis_data.defined_functions.empty()) {
            for (size_t i = 0; i < analysis_data.defined_functions.size(); ++i) {
                const auto& func = analysis_data.defined_functions[i];
                file << "function" << i << "_start=0x" << std::hex << func.first << std::dec << "\n";
                file << "function" << i << "_end=0x" << std::hex << func.second << std::dec << "\n";
            }
        }
        
        modified = false;
        return true;
    }
    
    void close() {
        clear();
        project_file_path.clear();
        loaded = false;
        modified = false;
    }
    
    std::string get_name() const {
        return project_info.name;
    }
    
    bool is_loaded() const {
        return loaded;
    }
    
    bool is_modified() const {
        return modified;
    }
    
    // Project manipulation methods
    void set_project_info(const std::string& name, const std::string& description = "") {
        project_info.name = name;
        project_info.description = description;
        modified = true;
    }
    
    void set_binary_path(const std::string& path) {
        project_info.binary_path = path;
        modified = true;
    }
    
    void set_symbol_file(const std::string& path) {
        project_info.symbol_file = path;
        modified = true;
    }
    
    ProjectInfo get_project_info() const {
        return project_info;
    }
    
    // Breakpoint management
    void add_breakpoint(uint64_t address, const std::string& type = "software", 
                       bool enabled = true, const std::string& condition = "",
                       const std::string& name = "") {
        BreakpointInfo bp;
        bp.address = address;
        bp.type = type;
        bp.enabled = enabled;
        bp.condition = condition;
        bp.name = name.empty() ? ("bp_" + std::to_string(address)) : name;
        
        breakpoints.push_back(bp);
        modified = true;
    }
    
    void remove_breakpoint(uint64_t address) {
        breakpoints.erase(
            std::remove_if(breakpoints.begin(), breakpoints.end(),
                          [address](const BreakpointInfo& bp) {
                              return bp.address == address;
                          }),
            breakpoints.end());
        modified = true;
    }
    
    std::vector<BreakpointInfo> get_breakpoints() const {
        return breakpoints;
    }
    
    // Analysis data management
    void add_analyzed_function(const std::string& function_name) {
        if (std::find(analysis_data.analyzed_functions.begin(), 
                     analysis_data.analyzed_functions.end(), 
                     function_name) == analysis_data.analyzed_functions.end()) {
            analysis_data.analyzed_functions.push_back(function_name);
            modified = true;
        }
    }
    
    void add_comment(uint64_t address, const std::string& comment) {
        analysis_data.comments[address] = comment;
        modified = true;
    }
    
    void add_label(uint64_t address, const std::string& label) {
        analysis_data.labels[address] = label;
        modified = true;
    }
    
    void add_function_definition(uint64_t start_addr, uint64_t end_addr) {
        analysis_data.defined_functions.push_back({start_addr, end_addr});
        modified = true;
    }
    
    AnalysisData get_analysis_data() const {
        return analysis_data;
    }
    
    // Utility methods
    std::string get_project_file_path() const {
        return project_file_path;
    }
    
    bool auto_save() {
        if (loaded && modified && !project_file_path.empty()) {
            return save(project_file_path);
        }
        return false;
    }

private:
    void clear() {
        project_info = ProjectInfo();
        project_info.version = "1.0";
        breakpoints.clear();
        analysis_data = AnalysisData();
    }
    
    void parse_project_data(const std::string& section, const std::string& key, const std::string& value) {
        if (section == "project") {
            if (key == "name") project_info.name = value;
            else if (key == "description") project_info.description = value;
            else if (key == "binary_path") project_info.binary_path = value;
            else if (key == "symbol_file") project_info.symbol_file = value;
            else if (key == "created_date") project_info.created_date = value;
            else if (key == "last_modified") project_info.last_modified = value;
            else if (key == "version") project_info.version = value;
        }
        else if (section == "breakpoints") {
            parse_breakpoint_data(key, value);
        }
        else if (section == "analysis") {
            parse_analysis_data(key, value);
        }
    }
    
    void parse_breakpoint_data(const std::string& key, const std::string& value) {
        // Extract breakpoint index from key like "bp0_address"
        size_t underscore_pos = key.find('_');
        if (underscore_pos == std::string::npos) return;
        
        std::string bp_prefix = key.substr(0, underscore_pos);
        std::string property = key.substr(underscore_pos + 1);
        
        // Get or create breakpoint
        int bp_index = std::stoi(bp_prefix.substr(2)); // Remove "bp" prefix
        
        // Ensure breakpoints vector is large enough
        while (static_cast<int>(breakpoints.size()) <= bp_index) {
            breakpoints.push_back(BreakpointInfo());
        }
        
        BreakpointInfo& bp = breakpoints[bp_index];
        
        if (property == "address") {
            bp.address = std::stoull(value, nullptr, 16);
        } else if (property == "type") {
            bp.type = value;
        } else if (property == "enabled") {
            bp.enabled = (value == "true");
        } else if (property == "condition") {
            bp.condition = value;
        } else if (property == "name") {
            bp.name = value;
        }
    }
    
    void parse_analysis_data(const std::string& key, const std::string& value) {
        if (key == "analyzed_functions") {
            // Parse comma-separated list
            std::istringstream iss(value);
            std::string function;
            while (std::getline(iss, function, ',')) {
                analysis_data.analyzed_functions.push_back(function);
            }
        } else if (key.substr(0, 8) == "comment_") {
            uint64_t address = std::stoull(key.substr(8), nullptr, 16);
            analysis_data.comments[address] = value;
        } else if (key.substr(0, 6) == "label_") {
            uint64_t address = std::stoull(key.substr(6), nullptr, 16);
            analysis_data.labels[address] = value;
        } else if (key.find("_start") != std::string::npos) {
            // Handle function definitions later when we have both start and end
        }
    }
    
    std::string get_current_timestamp() {
        auto now = std::time(nullptr);
        auto tm = *std::localtime(&now);
        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }
};

} // namespace debugger 