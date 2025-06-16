#include "disassembler.h"

namespace debugger {

// Architecture-specific functionality stubs
class ArchitectureHelper {
public:
    static std::string get_register_name(Architecture arch, int reg_id) {
        switch (arch) {
            case Architecture::X86_64:
                switch (reg_id) {
                    case 0: return "rax";
                    case 1: return "rcx";
                    case 2: return "rdx";
                    case 3: return "rbx";
                    case 4: return "rsp";
                    case 5: return "rbp";
                    case 6: return "rsi";
                    case 7: return "rdi";
                    default: return "unknown";
                }
            case Architecture::X86:
                switch (reg_id) {
                    case 0: return "eax";
                    case 1: return "ecx";
                    case 2: return "edx";
                    case 3: return "ebx";
                    case 4: return "esp";
                    case 5: return "ebp";
                    case 6: return "esi";
                    case 7: return "edi";
                    default: return "unknown";
                }
            default:
                return "unknown";
        }
    }
    
    static size_t get_pointer_size(Architecture arch) {
        switch (arch) {
            case Architecture::X86_64:
            case Architecture::ARM64:
                return 8;
            case Architecture::X86:
            case Architecture::ARM:
                return 4;
            default:
                return 8;
        }
    }
};

} // namespace debugger 