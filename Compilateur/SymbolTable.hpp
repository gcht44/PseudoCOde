#ifndef SYMBOLTABLE_HPP
#define SYMBOLTABLE_HPP

#include <map>
#include <iostream>

enum class Type {
    ENTIER,
    REEL,
    BOOL,
    STRING,
    NONE,
};

struct VariableInfo {
    Type type;
    // Autres informations (scope, valeur initiale, etc.)
};

class SymbolTable {
private:
    std::map<std::string, VariableInfo> table;

public:
    void addVariable(const std::string& name, Type type);

    Type getVariableType(const std::string& name) const;

};

#endif