#include "decompiler.h"

namespace debugger {

// AST builder stubs for decompiler
class ASTBuilder {
public:
    static std::string buildAST(const std::vector<Instruction>& instructions) {
        return "// AST not implemented yet";
    }
    
    static std::string convertToC(const std::string& ast) {
        return ast;
    }
};

} // namespace debugger 