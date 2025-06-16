#include "debugger_engine.h"

namespace debugger {

// Breakpoint management stubs
class BreakpointManager {
public:
    static bool addBreakpoint(uint64_t address) {
        return true;
    }
    
    static bool removeBreakpoint(uint64_t address) {
        return true;
    }
    
    static std::vector<uint64_t> getBreakpoints() {
        return {};
    }
};

} // namespace debugger 