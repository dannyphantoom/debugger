#include <string>
#include <vector>
#include <cstdint>
#include <cstdio>

namespace debugger {

// Utility functions for the debugger
std::string format_hex_address(uint64_t address) {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "0x%016lx", address);
    return std::string(buffer);
}

std::string format_hex_bytes(const std::vector<uint8_t>& bytes) {
    std::string result;
    for (size_t i = 0; i < bytes.size(); ++i) {
        char buffer[4];
        snprintf(buffer, sizeof(buffer), "%02x", bytes[i]);
        result += buffer;
        if (i < bytes.size() - 1) {
            result += " ";
        }
    }
    return result;
}

} // namespace debugger 