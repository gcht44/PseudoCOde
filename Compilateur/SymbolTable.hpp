#ifndef SYMBOLTABLE_HPP
#define SYMBOLTABLE_HPP

#include <map>
#include <iostream>
#include <vector>

enum class Type {
    ENTIER,
    REEL,
    BOOL,
    STRING,
    NONE,
};

struct VariableInfo {
    Type type;
    /*bool isArray;
    int arraySize;
    std::vector<int> valuesInt;
    std::vector<float> valuesFloat;
    std::vector <std::string> valuesStr;
    // Autres informations (scope, valeur initiale, etc.)

    VariableInfo(Type tpe, bool array) : type(tpe), isArray(array) {}
    VariableInfo(Type tpe, bool array, int arrayS, std::vector<int> v) : type(tpe), isArray(array), arraySize(arrayS), valuesInt(v) {}
    VariableInfo(Type tpe, bool array, int arrayS, std::vector<float> v) : type(tpe), isArray(array), arraySize(arrayS), valuesFloat(v) {}
    VariableInfo(Type tpe, bool array, int arrayS, std::vector<std::string> v) : type(tpe), isArray(array), arraySize(arrayS), valuesStr(v) {}*/
};

class SymbolTable {
private:
    std::map<std::string, VariableInfo> table;

public:
    void addVariable(const std::string& name, Type type);

    Type getVariableType(const std::string& name) const;

};

#endif