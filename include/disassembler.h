#pragma once

#include <memory>
#include <string>
#include <vector>
#include <cstdint>
#include <capstone/capstone.h>

namespace debugger {

enum class Architecture {
    X86,
    X86_64,
    ARM,
    ARM64,
    UNKNOWN
};

struct Instruction {
    uint64_t address;
    std::string mnemonic;
    std::string operands;
    std::vector<uint8_t> bytes;
    size_t size;
    bool is_jump;
    bool is_call;
    bool is_return;
    uint64_t target_address;  // For jumps/calls
};

struct Function {
    uint64_t start_address;
    uint64_t end_address;
    std::string name;
    std::vector<Instruction> instructions;
    std::vector<uint64_t> cross_references;
};

class Disassembler {
public:
    explicit Disassembler(Architecture arch = Architecture::X86_64);
    ~Disassembler();

    // Core disassembly functions
    bool initialize(Architecture arch);
    std::vector<Instruction> disassemble(const uint8_t* data, size_t size, uint64_t base_address = 0);
    std::vector<Instruction> disassemble_range(const uint8_t* data, size_t size, 
                                             uint64_t start_addr, uint64_t end_addr);
    
    // Function analysis
    std::vector<Function> analyze_functions(const std::vector<Instruction>& instructions);
    Function analyze_function(const std::vector<Instruction>& instructions, uint64_t start_address);
    
    // Utility functions
    Architecture get_architecture() const;
    std::string get_architecture_name() const;
    bool is_valid_instruction(uint64_t address) const;
    
    // Cross-reference analysis
    std::vector<uint64_t> find_cross_references(uint64_t address, 
                                               const std::vector<Instruction>& instructions);
    
    // String and constant detection
    std::vector<std::string> extract_strings(const uint8_t* data, size_t size);
    std::vector<uint64_t> find_constants(const std::vector<Instruction>& instructions);

private:
    csh cs_handle;
    Architecture current_arch;
    bool initialized;
    
    // Helper functions
    bool setup_capstone(Architecture arch);
    void cleanup_capstone();
    Instruction convert_cs_instruction(const cs_insn* insn) const;
    bool is_function_start(const Instruction& insn) const;
    bool is_function_end(const Instruction& insn) const;
    uint64_t extract_target_address(const Instruction& insn) const;
};

} // namespace debugger 