#include "SymbolTable.hpp"

void SymbolTable::addVariable(const std::string& name, Type type) {
    table[name] = { type };
}

Type SymbolTable::getVariableType(const std::string& name) const {
    if (table.find(name) != table.end()) {
        return table.at(name).type;
    }
    throw "Erreur: variable non déclarée";
}