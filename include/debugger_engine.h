#pragma once

#include "disassembler.h"
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <sys/types.h>

namespace debugger {

enum class BreakpointType {
    SOFTWARE,
    HARDWARE,
    CONDITIONAL
};

enum class DebuggerState {
    NOT_RUNNING,
    RUNNING,
    PAUSED,
    STOPPED,
    ERROR
};

struct Register {
    std::string name;
    uint64_t value;
    size_t size;  // in bytes
    bool modified;
};

struct MemoryRegion {
    uint64_t start_address;
    uint64_t end_address;
    std::string permissions;
    std::string name;
    std::vector<uint8_t> data;
};

struct Breakpoint {
    uint64_t address;
    BreakpointType type;
    bool enabled;
    std::string condition;
    std::function<bool()> condition_func;
    uint8_t original_byte;  // For software breakpoints
    std::string name;
    size_t hit_count;
};

struct StackFrame {
    uint64_t return_address;
    uint64_t frame_pointer;
    uint64_t stack_pointer;
    std::string function_name;
    std::map<std::string, uint64_t> local_variables;
};

class DebuggerEngine {
public:
    DebuggerEngine();
    ~DebuggerEngine();

    // Process control
    bool attach_to_process(pid_t pid);
    bool load_executable(const std::string& path, const std::vector<std::string>& args = {});
    bool start_process();
    bool continue_execution();
    bool pause_execution();
    bool stop_execution();
    bool detach();

    // Stepping
    bool step_into();
    bool step_over();
    bool step_out();
    bool step_instruction();

    // Breakpoints
    bool add_breakpoint(uint64_t address, BreakpointType type = BreakpointType::SOFTWARE);
    bool add_conditional_breakpoint(uint64_t address, const std::string& condition);
    bool remove_breakpoint(uint64_t address);
    bool enable_breakpoint(uint64_t address);
    bool disable_breakpoint(uint64_t address);
    std::vector<Breakpoint> get_breakpoints() const;
    bool is_breakpoint_hit(uint64_t address) const;

    // Memory operations
    std::vector<uint8_t> read_memory(uint64_t address, size_t size);
    bool write_memory(uint64_t address, const std::vector<uint8_t>& data);
    std::vector<MemoryRegion> get_memory_regions();
    bool set_memory_protection(uint64_t address, size_t size, const std::string& permissions);

    // Register operations
    std::vector<Register> get_registers();
    bool set_register(const std::string& name, uint64_t value);
    uint64_t get_register_value(const std::string& name);
    uint64_t get_instruction_pointer();
    uint64_t get_stack_pointer();
    uint64_t get_frame_pointer();

    // Stack operations
    std::vector<StackFrame> get_stack_trace();
    std::vector<uint64_t> get_stack_data(size_t frame_count = 20);

    // State management
    DebuggerState get_state() const;
    pid_t get_process_id() const;
    std::string get_last_error() const;

    // Event handling
    void set_breakpoint_callback(std::function<void(uint64_t)> callback);
    void set_signal_callback(std::function<void(int)> callback);
    void set_exit_callback(std::function<void(int)> callback);

    // Utility functions
    bool is_process_running() const;
    std::string get_current_function_name(uint64_t address = 0);
    uint64_t resolve_symbol(const std::string& symbol_name);
    std::vector<std::string> get_loaded_modules();

private:
    pid_t target_pid;
    DebuggerState current_state;
    std::string executable_path;
    std::vector<std::string> program_args;
    std::map<uint64_t, Breakpoint> breakpoints;
    std::string last_error;
    
    // Callbacks
    std::function<void(uint64_t)> breakpoint_callback;
    std::function<void(int)> signal_callback;
    std::function<void(int)> exit_callback;

    // Platform-specific data
    void* platform_data;  // For storing platform-specific debugging context

    // Helper functions
    bool setup_debugging();
    bool cleanup_debugging();
    bool wait_for_signal();
    bool handle_breakpoint(uint64_t address);
    bool insert_software_breakpoint(uint64_t address);
    bool remove_software_breakpoint(uint64_t address);
    bool evaluate_condition(const std::string& condition);
    void update_register_cache();
    bool is_valid_address(uint64_t address);
    std::string get_protection_string(int prot);
};

} // namespace debugger 