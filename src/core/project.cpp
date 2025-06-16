#include <string>
#include <vector>

namespace debugger {

// Project management stubs
class Project {
public:
    bool load(const std::string& filename) { return false; }
    bool save(const std::string& filename) { return false; }
    void close() {}
    std::string get_name() const { return ""; }
    bool is_loaded() const { return false; }
};

} // namespace debugger 