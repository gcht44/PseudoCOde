#ifndef AST_HPP
#define AST_HPP

#include <iostream>
#include <vector>
#include "SymbolTable.hpp"

std::string printType(Type type);

// Classe abstraite

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void print(int indent = 0) const = 0;
    virtual Type checkType(SymbolTable& symbolTable) const = 0;
};

// Noeud pour un ENTIER
class IntNode : public ASTNode{
private:
    std::string value;
public:
    IntNode(std::string v) : value(v) {}
    void print(int indent = 0) const override;
    std::string getValue() const;
    Type checkType(SymbolTable& symbolTable) const override;
};

// Noeud pour un REEL
class ReelNode : public ASTNode {
private:
    std::string value;
public:
    ReelNode(std::string v) : value(v) {}
    void print(int indent = 0) const override;
    std::string getValue() const;
    Type checkType(SymbolTable& symbolTable) const override;
};

// Noeud pour un BOOLEAN
class BoolNode : public ASTNode {
private:
    std::string value;
public:
    BoolNode(std::string v) : value(v) {}
    void print(int indent = 0) const override;
    std::string getValue() const;
    Type checkType(SymbolTable& symbolTable) const override;
};

// Noeud pour un STRING
class StringNode : public ASTNode {
private:
    std::string value;
public:
    StringNode(std::string v) : value(v) {}
    void print(int indent = 0) const override;
    std::string getValue() const;
    Type checkType(SymbolTable& symbolTable) const override;
};

// Noeud pour une variable (identifiant)
class IdentifierNode : public ASTNode {
private:
    std::string name;
public:
    IdentifierNode(const std::string& n) : name(n) {}
    void print(int indent = 0) const override;
    std::string getName() const;
    Type checkType(SymbolTable& symbolTable) const override;
};

// Noeud pour une opération binaire (ex: x + 2)
class BinaryOpNode : public ASTNode {
    std::string op; // Opérateur (+, -, *, /, etc.)
    std::unique_ptr<ASTNode> left;  // Sous-arbre gauche
    std::unique_ptr<ASTNode> right; // Sous-arbre droit
public:
    BinaryOpNode(std::string o, std::unique_ptr<ASTNode> l, std::unique_ptr<ASTNode> r)
        : op(o), left(std::move(l)), right(std::move(r)) {
    }
    void print(int indent = 0) const override;
    std::string getOp() const;
    const std::unique_ptr<ASTNode>& getLeft() const;
    const std::unique_ptr<ASTNode>& getRight() const;
    Type checkType(SymbolTable& symbolTable) const override;
};

// Noeud pour une déclaration de variable (let x = ...)
class VarDeclarationNode : public ASTNode {
    std::string name;
    std::unique_ptr<ASTNode> expression;
public:
    VarDeclarationNode(const std::string& n, std::unique_ptr<ASTNode> expr)
        : name(n), expression(std::move(expr)) {
    }
    void print(int indent = 0) const override;
    std::string getName() const;
    const std::unique_ptr<ASTNode>& getExpr() const;
    Type checkType(SymbolTable& symbolTable) const override;
};

// Noeud pour une assignation (x = y + 1)
class AssignmentNode : public ASTNode {
    std::string name;
    std::unique_ptr<ASTNode> expression;
public:
    AssignmentNode(const std::string& n, std::unique_ptr<ASTNode> expr)
        : name(n), expression(std::move(expr)) {
    }
    void print(int indent = 0) const override;
    std::string getName() const;
    const std::unique_ptr<ASTNode>& getExpr() const;
    Type checkType(SymbolTable& symbolTable) const override;
};

// Noeud pour une instruction print (print x)
class PrintNode : public ASTNode {
    std::unique_ptr<ASTNode> expression;
public:
    PrintNode(std::unique_ptr<ASTNode> expr) : expression(std::move(expr)) {}
    void print(int indent = 0) const override;
    const std::unique_ptr<ASTNode>& getExpr() const;
    Type checkType(SymbolTable& symbolTable) const;
};

// Noeud pour une instruction if 
class IfNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> condition; // La condition (expression)
    std::unique_ptr<ASTNode> ifBlock; // Bloc de code si la condition est vraie
    std::unique_ptr<ASTNode> elseBlock; // Bloc de code si la condition est fausse (optionnel)

    IfNode(std::unique_ptr<ASTNode> cond, std::unique_ptr<ASTNode> ifBlk, std::unique_ptr<ASTNode> elseBlk = nullptr)
        : condition(std::move(cond)), ifBlock(std::move(ifBlk)), elseBlock(std::move(elseBlk)) {}

    void print(int indent = 0) const override;
    const std::unique_ptr<ASTNode>& getCond() const;
    const std::unique_ptr<ASTNode>& getIfBlock() const;
    const std::unique_ptr<ASTNode>& getElseBlock() const;
    Type checkType(SymbolTable& symbolTable) const;
};

// Noeud pour une instruction block
class BlockNode : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> Block; 

    BlockNode(std::vector<std::unique_ptr<ASTNode>> block)
        : Block(std::move(block)) {
    }

    void print(int indent = 0) const override;
    const std::vector<std::unique_ptr<ASTNode>>& getBlock() const;
    Type checkType(SymbolTable& symbolTable) const { return Type::NONE; }
};


// Noeud racine contenant toutes les instructions
class ProgramNode : public ASTNode {
private:
    std::vector<std::unique_ptr<ASTNode>> statements;
public:
    void addStatement(std::unique_ptr<ASTNode> stmt);
    void print(int indent = 0) const override;  
    const std::vector<std::unique_ptr<ASTNode>>& getTableAST() const;
    Type checkType(SymbolTable& symbolTable) const;
};

#endif