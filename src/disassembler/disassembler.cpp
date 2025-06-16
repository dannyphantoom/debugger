#include "disassembler.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace debugger {

Disassembler::Disassembler(Architecture arch) 
    : cs_handle(0), current_arch(Architecture::UNKNOWN), initialized(false) {
    initialize(arch);
}

Disassembler::~Disassembler() {
    cleanup_capstone();
}

bool Disassembler::initialize(Architecture arch) {
    cleanup_capstone();
    
    if (setup_capstone(arch)) {
        current_arch = arch;
        initialized = true;
        return true;
    }
    
    return false;
}

bool Disassembler::setup_capstone(Architecture arch) {
    cs_arch cs_arch_mode;
    cs_mode cs_mode_flags;
    
    switch (arch) {
        case Architecture::X86:
            cs_arch_mode = CS_ARCH_X86;
            cs_mode_flags = CS_MODE_32;
            break;
        case Architecture::X86_64:
            cs_arch_mode = CS_ARCH_X86;
            cs_mode_flags = CS_MODE_64;
            break;
        case Architecture::ARM:
            cs_arch_mode = CS_ARCH_ARM;
            cs_mode_flags = CS_MODE_ARM;
            break;
        case Architecture::ARM64:
            cs_arch_mode = CS_ARCH_ARM64;
            cs_mode_flags = CS_MODE_ARM;
            break;
        default:
            return false;
    }
    
    cs_err err = cs_open(cs_arch_mode, cs_mode_flags, &cs_handle);
    if (err != CS_ERR_OK) {
        std::cerr << "Failed to initialize Capstone: " << cs_strerror(err) << std::endl;
        return false;
    }
    
    // Enable detailed instruction information
    cs_option(cs_handle, CS_OPT_DETAIL, CS_OPT_ON);
    
    return true;
}

void Disassembler::cleanup_capstone() {
    if (cs_handle != 0) {
        cs_close(&cs_handle);
        cs_handle = 0;
    }
    initialized = false;
}

std::vector<Instruction> Disassembler::disassemble(const uint8_t* data, size_t size, uint64_t base_address) {
    std::vector<Instruction> instructions;
    
    if (!initialized || !data || size == 0) {
        return instructions;
    }
    
    cs_insn* insn;
    size_t count = cs_disasm(cs_handle, data, size, base_address, 0, &insn);
    
    if (count > 0) {
        instructions.reserve(count);
        
        for (size_t i = 0; i < count; i++) {
            instructions.push_back(convert_cs_instruction(&insn[i]));
        }
        
        cs_free(insn, count);
    }
    
    return instructions;
}

std::vector<Instruction> Disassembler::disassemble_range(const uint8_t* data, size_t size, 
                                                        uint64_t start_addr, uint64_t end_addr) {
    std::vector<Instruction> all_instructions = disassemble(data, size, start_addr);
    std::vector<Instruction> filtered_instructions;
    
    for (const auto& insn : all_instructions) {
        if (insn.address >= start_addr && insn.address < end_addr) {
            filtered_instructions.push_back(insn);
        }
    }
    
    return filtered_instructions;
}

Instruction Disassembler::convert_cs_instruction(const cs_insn* insn) const {
    Instruction instruction;
    
    instruction.address = insn->address;
    instruction.mnemonic = insn->mnemonic;
    instruction.operands = insn->op_str;
    instruction.size = insn->size;
    
    // Copy instruction bytes
    instruction.bytes.resize(insn->size);
    std::copy(insn->bytes, insn->bytes + insn->size, instruction.bytes.begin());
    
    // Analyze instruction type
    instruction.is_jump = false;
    instruction.is_call = false;
    instruction.is_return = false;
    instruction.target_address = 0;
    
    if (insn->detail) {
        // Check for control flow instructions
        for (int i = 0; i < insn->detail->groups_count; i++) {
            uint8_t group = insn->detail->groups[i];
            
            switch (group) {
                case CS_GRP_JUMP:
                    instruction.is_jump = true;
                    break;
                case CS_GRP_CALL:
                    instruction.is_call = true;
                    break;
                case CS_GRP_RET:
                    instruction.is_return = true;
                    break;
            }
        }
        
        // Extract target address for jumps and calls
        if (instruction.is_jump || instruction.is_call) {
            instruction.target_address = extract_target_address(instruction);
        }
    }
    
    return instruction;
}

uint64_t Disassembler::extract_target_address(const Instruction& insn) const {
    // Simple implementation - extract address from operand string
    // This is a basic approach; a more sophisticated implementation would
    // parse the operands properly using Capstone's detail information
    
    std::string operands = insn.operands;
    if (operands.find("0x") != std::string::npos) {
        size_t pos = operands.find("0x");
        std::string hex_str = operands.substr(pos + 2);
        
        // Find end of hex number
        size_t end_pos = 0;
        while (end_pos < hex_str.length() && 
               std::isxdigit(hex_str[end_pos])) {
            end_pos++;
        }
        
        if (end_pos > 0) {
            hex_str = hex_str.substr(0, end_pos);
            try {
                return std::stoull(hex_str, nullptr, 16);
            } catch (...) {
                return 0;
            }
        }
    }
    
    return 0;
}

std::vector<Function> Disassembler::analyze_functions(const std::vector<Instruction>& instructions) {
    std::vector<Function> functions;
    
    if (instructions.empty()) {
        return functions;
    }
    
    Function current_function;
    bool in_function = false;
    
    for (size_t i = 0; i < instructions.size(); i++) {
        const auto& insn = instructions[i];
        
        // Check if this is a function start
        if (!in_function && is_function_start(insn)) {
            current_function = Function();
            current_function.start_address = insn.address;
            current_function.name = "sub_" + std::to_string(insn.address);
            in_function = true;
        }
        
        if (in_function) {
            current_function.instructions.push_back(insn);
            
            // Check if this is a function end
            if (is_function_end(insn)) {
                current_function.end_address = insn.address + insn.size;
                functions.push_back(current_function);
                in_function = false;
            }
        }
    }
    
    // Handle case where function doesn't end with return
    if (in_function) {
        current_function.end_address = instructions.back().address + instructions.back().size;
        functions.push_back(current_function);
    }
    
    return functions;
}

Function Disassembler::analyze_function(const std::vector<Instruction>& instructions, uint64_t start_address) {
    Function function;
    function.start_address = start_address;
    function.name = "sub_" + std::to_string(start_address);
    
    bool found_start = false;
    for (const auto& insn : instructions) {
        if (insn.address == start_address) {
            found_start = true;
        }
        
        if (found_start) {
            function.instructions.push_back(insn);
            
            if (is_function_end(insn)) {
                function.end_address = insn.address + insn.size;
                break;
            }
        }
    }
    
    if (function.end_address == 0 && !function.instructions.empty()) {
        function.end_address = function.instructions.back().address + function.instructions.back().size;
    }
    
    return function;
}

bool Disassembler::is_function_start(const Instruction& insn) const {
    // Basic heuristics for function detection
    const std::string& mnemonic = insn.mnemonic;
    
    // x86/x64 function prologue patterns
    if (current_arch == Architecture::X86 || current_arch == Architecture::X86_64) {
        return (mnemonic == "push" && insn.operands.find("bp") != std::string::npos) ||
               (mnemonic == "push" && insn.operands.find("rbp") != std::string::npos) ||
               (mnemonic == "sub" && insn.operands.find("sp") != std::string::npos);
    }
    
    // ARM function patterns
    if (current_arch == Architecture::ARM || current_arch == Architecture::ARM64) {
        return (mnemonic == "push" || mnemonic == "stp") &&
               (insn.operands.find("lr") != std::string::npos || 
                insn.operands.find("x30") != std::string::npos);
    }
    
    return false;
}

bool Disassembler::is_function_end(const Instruction& insn) const {
    return insn.is_return;
}

std::vector<uint64_t> Disassembler::find_cross_references(uint64_t address, 
                                                         const std::vector<Instruction>& instructions) {
    std::vector<uint64_t> xrefs;
    
    for (const auto& insn : instructions) {
        if ((insn.is_jump || insn.is_call) && insn.target_address == address) {
            xrefs.push_back(insn.address);
        }
    }
    
    return xrefs;
}

std::vector<std::string> Disassembler::extract_strings(const uint8_t* data, size_t size) {
    std::vector<std::string> strings;
    std::string current_string;
    
    for (size_t i = 0; i < size; i++) {
        if (std::isprint(data[i]) && data[i] != 0) {
            current_string += static_cast<char>(data[i]);
        } else {
            if (current_string.length() >= 4) {  // Minimum string length
                strings.push_back(current_string);
            }
            current_string.clear();
        }
    }
    
    return strings;
}

std::vector<uint64_t> Disassembler::find_constants(const std::vector<Instruction>& instructions) {
    std::vector<uint64_t> constants;
    
    for (const auto& insn : instructions) {
        // Look for immediate values in operands
        std::string operands = insn.operands;
        size_t pos = operands.find("0x");
        
        while (pos != std::string::npos) {
            size_t end_pos = pos + 2;
            while (end_pos < operands.length() && std::isxdigit(operands[end_pos])) {
                end_pos++;
            }
            
            if (end_pos > pos + 2) {
                std::string hex_str = operands.substr(pos + 2, end_pos - pos - 2);
                try {
                    uint64_t value = std::stoull(hex_str, nullptr, 16);
                    if (std::find(constants.begin(), constants.end(), value) == constants.end()) {
                        constants.push_back(value);
                    }
                } catch (...) {
                    // Ignore invalid hex values
                }
            }
            
            pos = operands.find("0x", end_pos);
        }
    }
    
    return constants;
}

Architecture Disassembler::get_architecture() const {
    return current_arch;
}

std::string Disassembler::get_architecture_name() const {
    switch (current_arch) {
        case Architecture::X86: return "x86";
        case Architecture::X86_64: return "x86-64";
        case Architecture::ARM: return "ARM";
        case Architecture::ARM64: return "ARM64";
        default: return "Unknown";
    }
}

bool Disassembler::is_valid_instruction(uint64_t address) const {
    // This would need access to the current instruction set
    // For now, return true as a placeholder
    return true;
}

} // namespace debugger 