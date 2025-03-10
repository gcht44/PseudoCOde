#include "AST.hpp"

std::string printType(Type type)
{
    switch (type)
    {
    case Type::ENTIER:
        return "ENTIER";
    case Type::REEL:
        return "REEL";
    case Type::BOOL:
        return "BOOLEAN";
    case Type::STRING:
        return "STRING";
    case Type::NONE:
        return "NONE";
    }
}

// Fonction utilitaire pour l'affichage indenté
void printIndent(int indent) 
{
    for (int i = 0; i < indent; ++i) std::cout << "  ";
}


void IntNode::print(int indent) const
{
    printIndent(indent);
    std::cout << "ENTIER: " << value << "\n";   
}
Type IntNode::checkType(SymbolTable& symbolTable) const
{
    return Type::ENTIER;
}
std::string IntNode::getValue() const
{
    return this->value;
}


void ReelNode::print(int indent) const
{
    printIndent(indent);
    std::cout << "REEL: " << value << "\n";
}
std::string ReelNode::getValue() const
{
    return this->value;
}
Type ReelNode::checkType(SymbolTable& symbolTable) const
{
    return Type::REEL;
}


void BoolNode::print(int indent) const
{
    printIndent(indent);
    std::cout << "BOOLEAN: " << this->value << "\n";
}
std::string BoolNode::getValue() const
{
    return this->value;
}
Type BoolNode::checkType(SymbolTable & symbolTable) const
{
    return Type::BOOL;
}

void StringNode::print(int indent) const
{
    printIndent(indent);
    std::cout << "BOOLEAN: " << this->value << "\n";
}
std::string StringNode::getValue() const
{
    return this->value;
}
Type StringNode::checkType(SymbolTable& symbolTable) const
{
    return Type::STRING;
}


// Noeud pour une variable (identifiant)
void IdentifierNode::print(int indent) const
{
    printIndent(indent);
    std::cout << "Identifier: " << name << "\n";
}
std::string IdentifierNode::getName() const
{
    return this->name;
}
Type IdentifierNode::checkType(SymbolTable& symbolTable) const
{
    return symbolTable.getVariableType(this->name);
}


// Noeud pour une opération binaire (ex: x + 2)
void BinaryOpNode::print(int indent) const 
{
    printIndent(indent);
    std::cout << "BinaryOp: " << op << "\n";
    left->print(indent + 1);
    right->print(indent + 1);
}
std::string BinaryOpNode::getOp() const
{
    return this->op;
}
const std::unique_ptr<ASTNode>& BinaryOpNode::getLeft() const
{
    return this->left;
}
const std::unique_ptr<ASTNode>& BinaryOpNode::getRight() const 
{
    return this->right;
}
Type BinaryOpNode::checkType(SymbolTable& symbolTable) const
{
    Type leftType = this->left->checkType(symbolTable);
    Type rightType = this->right->checkType(symbolTable);

    if (leftType != rightType) { std::cerr << "[AST] ERR: Erreur de type: les opérandes doivent être du même type (" << printType(leftType) << this->op << printType(rightType) << ")" << std::endl; exit(1); }
    if (op == "+" || op == "-" || op == "*" || op == "/") 
    {
        if (leftType != Type::ENTIER && leftType != Type::REEL) 
        {
            std::cerr << "[AST] ERR: Erreur de type: opération arithmétique sur des types non numériques" << std::endl; 
            exit(1);
        }
        return leftType; // Retourne le type de l'expression
    }
    else if (op == "&&" || op == "||") 
    {
        if (leftType != Type::BOOL) 
        {
            throw "Erreur de type: opération logique sur des types non booléens";
        }
        return Type::BOOL; // Retourne le type de l'expression
    }
    else 
    {
        throw "Erreur: opérateur non supporté";
    }
}


// Noeud pour une déclaration de variable (let x = ...)
void VarDeclarationNode::print(int indent) const
{
    printIndent(indent);
    std::cout << "VarDeclaration: " << name << "\n";
    expression->print(indent + 1);
}
std::string VarDeclarationNode::getName() const
{
    return this->name;
}
const std::unique_ptr<ASTNode>& VarDeclarationNode::getExpr() const
{
    return this->expression;
}
Type VarDeclarationNode::checkType(SymbolTable& symbolTable) const
{
    Type varType = symbolTable.getVariableType(this->name);
    Type valueType = this->expression->checkType(symbolTable);

    if (varType != valueType) { std::cerr << "[AST] ERR: Erreur de type: la valeur assignée (" << printType(valueType) << ") ne correspond pas au type de la variable (" << printType(varType) << ")"; exit(1); }
    return varType;
}


// Noeud pour une assignation (x = y + 1)
void AssignmentNode::print(int indent) const 
{
    printIndent(indent);
    std::cout << "Assignment: " << name << "\n";
    expression->print(indent + 1);
}
std::string AssignmentNode::getName() const 
{
    return this->name;
}
const std::unique_ptr<ASTNode>& AssignmentNode::getExpr() const
{
    return this->expression;
}
Type AssignmentNode::checkType(SymbolTable& symbolTable) const
{
    Type varType = symbolTable.getVariableType(this->name);
    Type valueType = this->expression->checkType(symbolTable);

    if (varType != valueType) { std::cerr << "[AST] ERR: Erreur de type: la valeur assignée (" << printType(valueType) << ") ne correspond pas au type de la variable (" << printType(varType) << ")"; exit(1); }
    return varType;
}


// Noeud pour une instruction print (print x)
void PrintNode::print(int indent) const 
{
    printIndent(indent);
    std::cout << "Print\n";
    expression->print(indent + 1);
}

const std::unique_ptr<ASTNode>& PrintNode::getExpr() const 
{
    return this->expression;
}
Type PrintNode::checkType(SymbolTable& symbolTable) const
{
    return this->expression->checkType(symbolTable);
}

// Noeud racine contenant toutes les instructions
void ProgramNode::addStatement(std::unique_ptr<ASTNode> stmt) 
{
    statements.push_back(std::move(stmt));
}


void ProgramNode::print(int indent) const {
    printIndent(indent); // Affiche l'indentation
    std::cout << "Program\n"; // Affiche le nom du nœud racine

    // Parcourt tous les nœuds enfants (instructions)
    for (const auto& stmt : statements) {
        stmt->print(indent + 1); // Appelle récursivement print() sur chaque nœud enfant
    }
}

const std::vector<std::unique_ptr<ASTNode>>& ProgramNode::getTableAST() const
{
    return this->statements;
}

Type ProgramNode::checkType(SymbolTable& symbolTable) const
{
    return Type::NONE;
}