#ifndef BYTECODE_HPP
#define BYTECODE_HPP

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include "AST.hpp"



enum Opcode {
    PUSH_CONST = 1,
    PUSH_VAR = 2,
    STORE_VAR = 3,
    ADD = 4,
    PRINT = 5,
    SUB = 6,
    MULT = 7,
    DIV = 8
};

struct Instruction {
    Opcode opcode;
    std::string arg;  // Argument optionnel (nom de variable pour PUSH_VAR et STORE_VAR)
    int value;        // Argument optionnel pour les constantes
};

class ByteCode {
private:
    std::vector<int> stackInt;
    std::vector<Instruction> bytecode;
    std::map<std::string, int> varIntTable;

    void generateBytecode(const ASTNode* node);
    void generateExpressionBytecode(const ASTNode* node);

    void pushStackInt(int value);
    int popStackInt();
public:
    ByteCode();
    void generateAllByteCode(const std::vector<std::unique_ptr<ASTNode>>& AST);
    void printByteCode();
    void executeByteCode();



};

#endif