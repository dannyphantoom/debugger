#include "debugger_engine.h"
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <iostream>

namespace debugger {

DebuggerEngine::DebuggerEngine() 
    : target_pid(-1), current_state(DebuggerState::NOT_RUNNING), platform_data(nullptr) {
}

DebuggerEngine::~DebuggerEngine() {
    cleanup_debugging();
}

bool DebuggerEngine::attach_to_process(pid_t pid) {
    if (ptrace(PTRACE_ATTACH, pid, nullptr, nullptr) == -1) {
        last_error = "Failed to attach to process";
        return false;
    }
    
    target_pid = pid;
    current_state = DebuggerState::PAUSED;
    
    // Wait for process to stop
    int status;
    waitpid(pid, &status, 0);
    
    return true;
}

bool DebuggerEngine::load_executable(const std::string& path, const std::vector<std::string>& args) {
    executable_path = path;
    program_args = args;
    return true;
}

bool DebuggerEngine::start_process() {
    if (executable_path.empty()) {
        last_error = "No executable loaded";
        return false;
    }
    
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        ptrace(PTRACE_TRACEME, 0, nullptr, nullptr);
        
        // Convert args to char* array
        std::vector<char*> argv;
        argv.push_back(const_cast<char*>(executable_path.c_str()));
        for (const auto& arg : program_args) {
            argv.push_back(const_cast<char*>(arg.c_str()));
        }
        argv.push_back(nullptr);
        
        execv(executable_path.c_str(), argv.data());
        exit(1); // If execv fails
    } else if (pid > 0) {
        // Parent process
        target_pid = pid;
        current_state = DebuggerState::PAUSED;
        
        // Wait for child to stop at first instruction
        int status;
        waitpid(pid, &status, 0);
        
        return true;
    } else {
        last_error = "Failed to fork process";
        return false;
    }
}

bool DebuggerEngine::continue_execution() {
    if (target_pid == -1) {
        last_error = "No process attached";
        return false;
    }
    
    if (ptrace(PTRACE_CONT, target_pid, nullptr, nullptr) == -1) {
        last_error = "Failed to continue execution";
        return false;
    }
    
    current_state = DebuggerState::RUNNING;
    return true;
}

bool DebuggerEngine::pause_execution() {
    if (target_pid == -1) {
        last_error = "No process attached";
        return false;
    }
    
    if (kill(target_pid, SIGSTOP) == -1) {
        last_error = "Failed to pause execution";
        return false;
    }
    
    current_state = DebuggerState::PAUSED;
    return true;
}

bool DebuggerEngine::stop_execution() {
    if (target_pid == -1) {
        last_error = "No process attached";
        return false;
    }
    
    if (kill(target_pid, SIGKILL) == -1) {
        last_error = "Failed to stop execution";
        return false;
    }
    
    current_state = DebuggerState::STOPPED;
    target_pid = -1;
    return true;
}

bool DebuggerEngine::detach() {
    if (target_pid == -1) {
        last_error = "No process attached";
        return false;
    }
    
    if (ptrace(PTRACE_DETACH, target_pid, nullptr, nullptr) == -1) {
        last_error = "Failed to detach from process";
        return false;
    }
    
    target_pid = -1;
    current_state = DebuggerState::NOT_RUNNING;
    return true;
}

bool DebuggerEngine::step_into() {
    if (target_pid == -1) {
        last_error = "No process attached";
        return false;
    }
    
    if (ptrace(PTRACE_SINGLESTEP, target_pid, nullptr, nullptr) == -1) {
        last_error = "Failed to single step";
        return false;
    }
    
    // Wait for step to complete
    int status;
    waitpid(target_pid, &status, 0);
    
    return true;
}

bool DebuggerEngine::step_over() {
    // For now, implement as single step
    // A proper implementation would check if next instruction is a call
    // and set a temporary breakpoint after it
    return step_into();
}

bool DebuggerEngine::step_out() {
    // Placeholder - would need to analyze stack and set breakpoint at return address
    last_error = "Step out not implemented yet";
    return false;
}

bool DebuggerEngine::step_instruction() {
    return step_into();
}

bool DebuggerEngine::add_breakpoint(uint64_t address, BreakpointType type) {
    if (target_pid == -1) {
        last_error = "No process attached";
        return false;
    }
    
    if (type == BreakpointType::SOFTWARE) {
        return insert_software_breakpoint(address);
    }
    
    // Hardware breakpoints not implemented yet
    last_error = "Hardware breakpoints not implemented";
    return false;
}

bool DebuggerEngine::insert_software_breakpoint(uint64_t address) {
    // Read original byte
    errno = 0;
    long data = ptrace(PTRACE_PEEKTEXT, target_pid, address, nullptr);
    if (errno != 0) {
        last_error = "Failed to read memory at breakpoint address";
        return false;
    }
    
    Breakpoint bp;
    bp.address = address;
    bp.type = BreakpointType::SOFTWARE;
    bp.enabled = true;
    bp.original_byte = data & 0xFF;
    bp.hit_count = 0;
    
    // Write INT3 instruction (0xCC)
    long new_data = (data & ~0xFF) | 0xCC;
    if (ptrace(PTRACE_POKETEXT, target_pid, address, new_data) == -1) {
        last_error = "Failed to write breakpoint instruction";
        return false;
    }
    
    breakpoints[address] = bp;
    return true;
}

bool DebuggerEngine::remove_software_breakpoint(uint64_t address) {
    auto it = breakpoints.find(address);
    if (it == breakpoints.end()) {
        last_error = "Breakpoint not found";
        return false;
    }
    
    // Restore original byte
    long data = ptrace(PTRACE_PEEKTEXT, target_pid, address, nullptr);
    long new_data = (data & ~0xFF) | it->second.original_byte;
    
    if (ptrace(PTRACE_POKETEXT, target_pid, address, new_data) == -1) {
        last_error = "Failed to restore original instruction";
        return false;
    }
    
    breakpoints.erase(it);
    return true;
}

std::vector<uint8_t> DebuggerEngine::read_memory(uint64_t address, size_t size) {
    std::vector<uint8_t> data;
    
    if (target_pid == -1) {
        return data;
    }
    
    data.reserve(size);
    
    for (size_t i = 0; i < size; i += sizeof(long)) {
        errno = 0;
        long word = ptrace(PTRACE_PEEKTEXT, target_pid, address + i, nullptr);
        if (errno != 0) {
            break;
        }
        
        // Copy bytes from word
        for (size_t j = 0; j < sizeof(long) && i + j < size; ++j) {
            data.push_back((word >> (j * 8)) & 0xFF);
        }
    }
    
    return data;
}

bool DebuggerEngine::write_memory(uint64_t address, const std::vector<uint8_t>& data) {
    if (target_pid == -1) {
        last_error = "No process attached";
        return false;
    }
    
    // Write memory word by word
    for (size_t i = 0; i < data.size(); i += sizeof(long)) {
        long word = 0;
        
        // Pack bytes into word
        for (size_t j = 0; j < sizeof(long) && i + j < data.size(); ++j) {
            word |= static_cast<long>(data[i + j]) << (j * 8);
        }
        
        if (ptrace(PTRACE_POKETEXT, target_pid, address + i, word) == -1) {
            last_error = "Failed to write memory";
            return false;
        }
    }
    
    return true;
}

uint64_t DebuggerEngine::get_instruction_pointer() {
    if (target_pid == -1) return 0;
    
    // This is architecture-specific
    // For x86-64, RIP is at offset 128 in user_regs_struct
    errno = 0;
    long rip = ptrace(PTRACE_PEEKUSER, target_pid, 8 * 16, nullptr); // RIP offset
    if (errno != 0) return 0;
    
    return static_cast<uint64_t>(rip);
}

// Stub implementations for remaining methods
bool DebuggerEngine::add_conditional_breakpoint(uint64_t, const std::string&) { return false; }
bool DebuggerEngine::remove_breakpoint(uint64_t address) { return remove_software_breakpoint(address); }
bool DebuggerEngine::enable_breakpoint(uint64_t) { return false; }
bool DebuggerEngine::disable_breakpoint(uint64_t) { return false; }
std::vector<Breakpoint> DebuggerEngine::get_breakpoints() const { 
    std::vector<Breakpoint> result;
    for (const auto& pair : breakpoints) {
        result.push_back(pair.second);
    }
    return result;
}
bool DebuggerEngine::is_breakpoint_hit(uint64_t address) const { 
    return breakpoints.find(address) != breakpoints.end();
}
std::vector<MemoryRegion> DebuggerEngine::get_memory_regions() { return {}; }
bool DebuggerEngine::set_memory_protection(uint64_t, size_t, const std::string&) { return false; }
std::vector<Register> DebuggerEngine::get_registers() { return {}; }
bool DebuggerEngine::set_register(const std::string&, uint64_t) { return false; }
uint64_t DebuggerEngine::get_register_value(const std::string&) { return 0; }
uint64_t DebuggerEngine::get_stack_pointer() { return 0; }
uint64_t DebuggerEngine::get_frame_pointer() { return 0; }
std::vector<StackFrame> DebuggerEngine::get_stack_trace() { return {}; }
std::vector<uint64_t> DebuggerEngine::get_stack_data(size_t) { return {}; }
DebuggerState DebuggerEngine::get_state() const { return current_state; }
pid_t DebuggerEngine::get_process_id() const { return target_pid; }
std::string DebuggerEngine::get_last_error() const { return last_error; }
void DebuggerEngine::set_breakpoint_callback(std::function<void(uint64_t)> callback) { breakpoint_callback = callback; }
void DebuggerEngine::set_signal_callback(std::function<void(int)> callback) { signal_callback = callback; }
void DebuggerEngine::set_exit_callback(std::function<void(int)> callback) { exit_callback = callback; }
bool DebuggerEngine::is_process_running() const { return target_pid != -1; }
std::string DebuggerEngine::get_current_function_name(uint64_t) { return ""; }
uint64_t DebuggerEngine::resolve_symbol(const std::string&) { return 0; }
std::vector<std::string> DebuggerEngine::get_loaded_modules() { return {}; }
bool DebuggerEngine::setup_debugging() { return true; }
bool DebuggerEngine::cleanup_debugging() { return true; }
bool DebuggerEngine::wait_for_signal() { return true; }
bool DebuggerEngine::handle_breakpoint(uint64_t) { return true; }
bool DebuggerEngine::evaluate_condition(const std::string&) { return true; }
void DebuggerEngine::update_register_cache() {}
bool DebuggerEngine::is_valid_address(uint64_t) { return true; }
std::string DebuggerEngine::get_protection_string(int) { return ""; }

} // namespace debugger 