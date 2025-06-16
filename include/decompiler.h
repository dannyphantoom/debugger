#pragma once

#include "disassembler.h"
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_set>

namespace debugger {

enum class VariableType {
    INT8, INT16, INT32, INT64,
    UINT8, UINT16, UINT32, UINT64,
    FLOAT, DOUBLE,
    POINTER,
    ARRAY,
    STRUCT,
    UNKNOWN
};

struct Variable {
    std::string name;
    VariableType type;
    uint64_t address;
    size_t size;
    bool is_parameter;
    bool is_local;
    std::string comment;
};

struct BasicBlock {
    uint64_t start_address;
    uint64_t end_address;
    std::vector<Instruction> instructions;
    std::vector<uint64_t> predecessors;
    std::vector<uint64_t> successors;
    std::string decompiled_code;
};

struct ControlFlow {
    enum Type { IF_ELSE, WHILE_LOOP, FOR_LOOP, SWITCH_CASE, SEQUENTIAL };
    Type type;
    uint64_t start_address;
    uint64_t end_address;
    std::string condition;
    std::vector<BasicBlock> blocks;
};

struct DecompiledFunction {
    std::string name;
    std::string return_type;
    std::vector<Variable> parameters;
    std::vector<Variable> local_variables;
    std::vector<ControlFlow> control_flows;
    std::string full_code;
    uint64_t start_address;
    uint64_t end_address;
};

class Decompiler {
public:
    explicit Decompiler(Architecture arch = Architecture::X86_64);
    ~Decompiler() = default;

    // Main decompilation functions
    DecompiledFunction decompile_function(const Function& function);
    std::string decompile_instruction_block(const std::vector<Instruction>& instructions);
    std::string decompile_single_instruction(const Instruction& instruction);

    // Analysis functions
    std::vector<BasicBlock> analyze_basic_blocks(const Function& function);
    std::vector<ControlFlow> analyze_control_flow(const std::vector<BasicBlock>& blocks);
    std::vector<Variable> analyze_variables(const Function& function);
    
    // Type inference
    VariableType infer_variable_type(const std::string& operand, const std::vector<Instruction>& context);
    std::string infer_function_signature(const Function& function);
    
    // Pattern matching
    bool match_loop_pattern(const std::vector<Instruction>& instructions, size_t start_idx);
    bool match_if_pattern(const std::vector<Instruction>& instructions, size_t start_idx);
    bool match_switch_pattern(const std::vector<Instruction>& instructions, size_t start_idx);
    
    // Code generation
    std::string generate_c_code(const DecompiledFunction& function);
    std::string generate_variable_declarations(const std::vector<Variable>& variables);
    std::string generate_control_flow_code(const ControlFlow& flow);
    
    // Configuration
    void set_architecture(Architecture arch);
    void enable_comments(bool enable);
    void set_variable_naming_style(const std::string& style);

private:
    Architecture current_arch;
    bool comments_enabled;
    std::string variable_naming_style;
    std::map<uint64_t, std::string> register_mappings;
    std::unordered_set<std::string> reserved_keywords;
    
    // Helper functions
    void initialize_register_mappings();
    void initialize_reserved_keywords();
    std::string sanitize_variable_name(const std::string& name);
    std::string get_register_name(const std::string& reg);
    std::string convert_operand_to_c(const std::string& operand);
    std::string generate_unique_variable_name(const std::string& base);
    bool is_arithmetic_instruction(const std::string& mnemonic);
    bool is_comparison_instruction(const std::string& mnemonic);
    bool is_memory_access(const std::string& mnemonic);
    std::string convert_condition_to_c(const std::string& condition);
    
    // Instruction conversion helpers
    std::string convert_mov_instruction(const std::string& operands);
    std::string convert_arithmetic_instruction(const std::string& op, const std::string& operands);
    std::string convert_call_instruction(const std::string& operands);
};

} // namespace debugger 