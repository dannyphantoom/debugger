#include "debugger_engine.h"

namespace debugger {

// Memory management stubs
class MemoryManager {
public:
    static std::vector<uint8_t> readMemory(uint64_t address, size_t size) {
        return {};
    }
    
    static bool writeMemory(uint64_t address, const std::vector<uint8_t>& data) {
        return true;
    }
    
    static std::vector<MemoryRegion> getMemoryMap() {
        return {};
    }
};

} // namespace debugger 