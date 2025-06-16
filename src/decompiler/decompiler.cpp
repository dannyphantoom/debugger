#include "decompiler.h"
#include <sstream>

namespace debugger {

Decompiler::Decompiler(Architecture arch) 
    : current_arch(arch), comments_enabled(true), variable_naming_style("v") {
    initialize_register_mappings();
    initialize_reserved_keywords();
}

DecompiledFunction Decompiler::decompile_function(const Function& function) {
    DecompiledFunction result;
    result.name = function.name;
    result.return_type = "int"; // Default
    result.start_address = function.start_address;
    result.end_address = function.end_address;
    
    // Basic implementation - convert each instruction to pseudo-C
    std::ostringstream code;
    code << result.return_type << " " << result.name << "() {\n";
    
    for (const auto& insn : function.instructions) {
        code << "    // " << insn.mnemonic << " " << insn.operands << "\n";
        std::string c_line = decompile_single_instruction(insn);
        if (!c_line.empty()) {
            code << "    " << c_line << ";\n";
        }
    }
    
    code << "}\n";
    result.full_code = code.str();
    
    return result;
}

std::string Decompiler::decompile_instruction_block(const std::vector<Instruction>& instructions) {
    std::ostringstream result;
    for (const auto& insn : instructions) {
        std::string line = decompile_single_instruction(insn);
        if (!line.empty()) {
            result << line << ";\n";
        }
    }
    return result.str();
}

std::string Decompiler::decompile_single_instruction(const Instruction& instruction) {
    const std::string& mnemonic = instruction.mnemonic;
    const std::string& operands = instruction.operands;
    
    // Basic x86/x64 instruction patterns
    if (mnemonic == "mov") {
        return convert_mov_instruction(operands);
    } else if (mnemonic == "add") {
        return convert_arithmetic_instruction("+=", operands);
    } else if (mnemonic == "sub") {
        return convert_arithmetic_instruction("-=", operands);
    } else if (mnemonic == "call") {
        return convert_call_instruction(operands);
    } else if (mnemonic == "ret") {
        return "return";
    } else if (mnemonic == "push") {
        return "/* push " + operands + " */";
    } else if (mnemonic == "pop") {
        return "/* pop " + operands + " */";
    }
    
    // Default: comment out unknown instructions
    return "/* " + mnemonic + " " + operands + " */";
}

std::string Decompiler::convert_mov_instruction(const std::string& operands) {
    // Parse "dest, src" format
    size_t comma_pos = operands.find(',');
    if (comma_pos == std::string::npos) return "";
    
    std::string dest = operands.substr(0, comma_pos);
    std::string src = operands.substr(comma_pos + 1);
    
    // Trim whitespace
    dest.erase(0, dest.find_first_not_of(" \t"));
    dest.erase(dest.find_last_not_of(" \t") + 1);
    src.erase(0, src.find_first_not_of(" \t"));
    src.erase(src.find_last_not_of(" \t") + 1);
    
    return convert_operand_to_c(dest) + " = " + convert_operand_to_c(src);
}

std::string Decompiler::convert_arithmetic_instruction(const std::string& op, const std::string& operands) {
    size_t comma_pos = operands.find(',');
    if (comma_pos == std::string::npos) return "";
    
    std::string dest = operands.substr(0, comma_pos);
    std::string src = operands.substr(comma_pos + 1);
    
    dest.erase(0, dest.find_first_not_of(" \t"));
    dest.erase(dest.find_last_not_of(" \t") + 1);
    src.erase(0, src.find_first_not_of(" \t"));
    src.erase(src.find_last_not_of(" \t") + 1);
    
    return convert_operand_to_c(dest) + " " + op + " " + convert_operand_to_c(src);
}

std::string Decompiler::convert_call_instruction(const std::string& operands) {
    std::string target = operands;
    target.erase(0, target.find_first_not_of(" \t"));
    target.erase(target.find_last_not_of(" \t") + 1);
    
    return convert_operand_to_c(target) + "()";
}

std::string Decompiler::convert_operand_to_c(const std::string& operand) {
    // Handle immediate values
    if (operand.find("0x") == 0) {
        return operand; // Keep hex literals as-is
    }
    
    // Handle memory references [reg+offset]
    if (operand.find('[') != std::string::npos) {
        return "*(" + get_register_name(operand) + ")";
    }
    
    // Handle registers
    return get_register_name(operand);
}

std::string Decompiler::get_register_name(const std::string& reg) {
    // Basic register mapping
    if (reg == "rax" || reg == "eax") return "var_a";
    if (reg == "rbx" || reg == "ebx") return "var_b";
    if (reg == "rcx" || reg == "ecx") return "var_c";
    if (reg == "rdx" || reg == "edx") return "var_d";
    if (reg == "rsi" || reg == "esi") return "var_si";
    if (reg == "rdi" || reg == "edi") return "var_di";
    if (reg == "rbp" || reg == "ebp") return "frame_ptr";
    if (reg == "rsp" || reg == "esp") return "stack_ptr";
    
    return "var_" + reg;
}

void Decompiler::initialize_register_mappings() {
    // Initialize x86/x64 register mappings
    register_mappings[0] = "var_a";  // RAX/EAX
    register_mappings[1] = "var_c";  // RCX/ECX
    register_mappings[2] = "var_d";  // RDX/EDX
    register_mappings[3] = "var_b";  // RBX/EBX
}

void Decompiler::initialize_reserved_keywords() {
    reserved_keywords = {
        "int", "char", "void", "return", "if", "else", "while", "for",
        "break", "continue", "switch", "case", "default", "struct",
        "union", "enum", "typedef", "static", "extern", "const"
    };
}

// Stub implementations for remaining methods
std::vector<BasicBlock> Decompiler::analyze_basic_blocks(const Function&) { return {}; }
std::vector<ControlFlow> Decompiler::analyze_control_flow(const std::vector<BasicBlock>&) { return {}; }
std::vector<Variable> Decompiler::analyze_variables(const Function&) { return {}; }
VariableType Decompiler::infer_variable_type(const std::string&, const std::vector<Instruction>&) { return VariableType::UNKNOWN; }
std::string Decompiler::infer_function_signature(const Function&) { return "int function()"; }
bool Decompiler::match_loop_pattern(const std::vector<Instruction>&, size_t) { return false; }
bool Decompiler::match_if_pattern(const std::vector<Instruction>&, size_t) { return false; }
bool Decompiler::match_switch_pattern(const std::vector<Instruction>&, size_t) { return false; }
std::string Decompiler::generate_c_code(const DecompiledFunction& func) { return func.full_code; }
std::string Decompiler::generate_variable_declarations(const std::vector<Variable>&) { return ""; }
std::string Decompiler::generate_control_flow_code(const ControlFlow&) { return ""; }
void Decompiler::set_architecture(Architecture arch) { current_arch = arch; }
void Decompiler::enable_comments(bool enable) { comments_enabled = enable; }
void Decompiler::set_variable_naming_style(const std::string& style) { variable_naming_style = style; }
std::string Decompiler::sanitize_variable_name(const std::string& name) { return name; }
std::string Decompiler::generate_unique_variable_name(const std::string& base) { return base + "_1"; }
bool Decompiler::is_arithmetic_instruction(const std::string& mnemonic) { 
    return mnemonic == "add" || mnemonic == "sub" || mnemonic == "mul" || mnemonic == "div"; 
}
bool Decompiler::is_comparison_instruction(const std::string& mnemonic) { 
    return mnemonic == "cmp" || mnemonic == "test"; 
}
bool Decompiler::is_memory_access(const std::string& mnemonic) { 
    return mnemonic == "mov" || mnemonic == "lea"; 
}
std::string Decompiler::convert_condition_to_c(const std::string& condition) { return condition; }

} // namespace debugger 