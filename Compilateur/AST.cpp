#include "AST.hpp"

// Fonction utilitaire pour l'affichage indenté
void printIndent(int indent) 
{
    for (int i = 0; i < indent; ++i) std::cout << "  ";
}


void NumberNode::print(int indent) const
{
    printIndent(indent);
    std::cout << "Number: " << value << "\n";   
}
std::string NumberNode::getValue() const
{
    return this->value;
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