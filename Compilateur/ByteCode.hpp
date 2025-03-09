#ifndef BYTECODE_HPP
#define BYTECODE_HPP

#include "AST.hpp"
#include "SymbolTable.hpp"

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <variant>
#include <iomanip>

enum Opcode {
    PUSH_CONST = 2,
    PUSH_VAR = 4,
    STORE_VAR = 5,
    ADD = 7,
    SUB = 9,
    MULT = 11,
    DIV = 13,
    PRINT = 15,
};

struct ValueVisitor {
    void operator()(std::monostate) const {
        std::cout << "None" << std::endl;
    }
    void operator()(int v) const {
        std::cout << v << std::endl;
    }
    void operator()(float v) const {
        std::cout << v << std::endl;
    }
    void operator()(bool v) const {
        std::cout << std::boolalpha << v << std::endl;
    }
    void operator()(const std::string& v) const {
        std::cout << v << std::endl;
    }
};

struct Value {
    Type type;
    std::variant<std::monostate, int, float, bool, std::string> data;


    explicit Value(int v) : type(Type::ENTIER), data(v) {}
    explicit Value(float v) : type(Type::REEL), data(v) {}
    explicit Value(bool v) : type(Type::BOOL), data(v) {}
    explicit Value(std::string v) : type(Type::STRING), data(std::move(v)) {}

    Value() : type(Type::NONE), data(std::monostate{}) {}  // Valeur par défaut

    void print() const {
        ValueVisitor visitor;
        std::visit(visitor, data);
    }
};

struct Instruction {
    Opcode opcode;
    Type type;
    std::string arg;  // Argument optionnel (nom de variable pour PUSH_VAR et STORE_VAR)
    Value value;

    // Instruction(Opcode op) : opcode(op), value() {}
    Instruction(Opcode op, Value val) : opcode(op), value(val) {}
    Instruction(Opcode op, std::string var) : opcode(op), arg(std::move(var)) {}
    Instruction(Opcode op, std::string var, Type t) : opcode(op), arg(std::move(var)), type(t) {}
    Instruction(Opcode op, Type t) : opcode(op), type(t) {}
};

class ByteCode {
private:
    std::vector<int> stackInt;
    std::vector<float> stackReel;
    std::vector<Instruction> bytecode;
    std::map<std::string, int> varIntTable;
    std::map<std::string, float> varReelTable;

    void generateBytecode(const ASTNode* node, SymbolTable& symbolTable);
    void generateExpressionBytecode(const ASTNode* node, SymbolTable& symbolTable);

    void pushStackInt(int value);
    void pushStackFloat(float value);
    void pushStackBool(bool value);
    void pushStackString(std::string value);

    int popStackInt();
    float popStackReel();
    bool popStackBool();
    std::string popStackString();
public:
    ByteCode();
    void generateAllByteCode(const std::vector<std::unique_ptr<ASTNode>>& AST, SymbolTable& symbolTable);
    void printByteCode();
    void executeByteCode();



};

#endif