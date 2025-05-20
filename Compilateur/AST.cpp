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
    std::cout << "STRING: " << this->value << "\n";
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
    else if (op == ".")
    {
        if (leftType != Type::STRING)
        {
            std::cerr << "[AST] ERR: Erreur de type: Concaténation possible seulement sur des types STRING" << std::endl;
            exit(1);
        }
        return leftType;
    }
    else if (op == ">" || op == "<" || op == ">=" || op == "<=" || op == "==" || op == "!=") {
        return Type::BOOL;
    }
    else if (op == "ET" || op == "OU") 
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


// Noeud pour une instruction if
void IfNode::print(int indent) const
{
    printIndent(indent);
    std::cout << "Si:\n";
    condition->print(indent); 

   // Afficher le bloc if
   printIndent(indent);
   std::cout << "ifBlock: " << std::endl;
   ifBlock->print(indent + 1);


    // Afficher le bloc else (s'il existe)
    if (elseBlock != nullptr) 
    {
        printIndent(indent);
        std::cout << "elseBlock: " << std::endl;
        elseBlock->print(indent + 1);
    }
}

const std::unique_ptr<ASTNode>& IfNode::getCond() const
{
    return this->condition;
}
const std::unique_ptr<ASTNode>& IfNode::getIfBlock() const
{
    return this->ifBlock;
}
const std::unique_ptr<ASTNode>& IfNode::getElseBlock() const
{
    return this->elseBlock;
}
Type IfNode::checkType(SymbolTable& symbolTable) const
{
    // Vérifie que la condition est un booléen
    if (condition->checkType(symbolTable) != Type::BOOL) {
        throw std::runtime_error("La condition doit être un booléen");
    }
    return Type::NONE; // Une condition n'a pas de type de retour
}

// Noeud pour une instruction TANTQUE
void TantQueNode::print(int indent) const
{
    printIndent(indent);
    std::cout << "TANTQUE:\n";
    condition->print(indent);

    // Afficher le bloc if
    printIndent(indent);
    std::cout << "TantQueBlock: " << std::endl;
    tantqueBlock->print(indent + 1);
}
const std::unique_ptr<ASTNode>& TantQueNode::getCond() const
{
    return this->condition;
}
const std::unique_ptr<ASTNode>& TantQueNode::getTantQueBlock() const
{
    return this->tantqueBlock;
}
Type TantQueNode::checkType(SymbolTable& symbolTable) const
{
    // Vérifie que la condition est un booléen
    if (condition->checkType(symbolTable) != Type::BOOL) {
        throw std::runtime_error("La condition doit être un booléen");
    }
    return Type::NONE; // Une condition n'a pas de type de retour
}



// Noeud pour une instruction POUR
void PourNode::print(int indent) const
{
    printIndent(indent);
    std::cout << "TANTQUE: ";
    this->nameVarInit;
    this->valDebut->print(indent);
    this->valFin->print(indent);

    // Afficher le bloc if
    printIndent(indent);
    std::cout << "PourBlock: " << std::endl;
    this->pourBlock->print(indent + 1);
}
const std::string& PourNode::getValInit() const
{
    return this->nameVarInit;
}
const std::unique_ptr<ASTNode>& PourNode::getValDebut() const
{
    return this->valDebut;
}
const std::unique_ptr<ASTNode>& PourNode::getValFin() const
{
    return this->valFin;
}
const std::unique_ptr<ASTNode>& PourNode::getPourBlock() const
{
    return this->pourBlock;
}
Type PourNode::checkType(SymbolTable& symbolTable) const
{
    Type varType = symbolTable.getVariableType(this->nameVarInit);
    // Vérifie que la condition est un booléen
    if (((valDebut->checkType(symbolTable) != Type::ENTIER) && (valFin->checkType(symbolTable) != Type::ENTIER)) != (varType != Type::ENTIER)) {
        throw std::runtime_error("Type attendu : ENTIER");
    }
    return Type::NONE; // Une condition n'a pas de type de retour
}


// Noeud pour une instruction TABLEAU
void ArrayDeclarationNode::print(int indent) const
{
    printIndent(indent);
    std::cout << "TABLEAU: \n";
    std::cout << "Index: ";
    this->index->print(indent);
    std::cout << "\n";
    for (int i = 0; i < this->elements.size(); i++) {
        this->elements[i]->print(indent + 1);
    }

}
const std::string& ArrayDeclarationNode::getName() const
{
    return this->name;
}
const std::vector<std::unique_ptr<ASTNode>>& ArrayDeclarationNode::getElems() const
{
    return this->elements;
}
const std::unique_ptr<ASTNode>& ArrayDeclarationNode::getIndex() const
{
    return this->index;
}
Type ArrayDeclarationNode::checkType(SymbolTable& symbolTable) const
{
    if (this->index->checkType(symbolTable) != Type::ENTIER)
    {
        std::cerr << "[AST]: La taille du tableau spécifier n'est pas un ENTIER";
        exit(1);
    }

    Type prevType = symbolTable.getVariableType(this->name);
    for (int i = 0; i < this->elements.size(); i++) {
        if (prevType != elements[i]->checkType(symbolTable))
        {
            std::cerr << "[AST]: Erreur de type dans le tableau";
            exit(1);
        }
    }
    return prevType; 
}



void ArrayAccesNode::print(int indent) const
{
    printIndent(indent);
    std::cout << "ArrayAccessNode: \n";
    printIndent(indent);
    std::cout << "Name: " + this->name << std::endl ;
}
const std::string& ArrayAccesNode::getName() const
{
    return this->name;
}
const std::unique_ptr<ASTNode>& ArrayAccesNode::getIndex() const
{
    return this->index;
}
Type ArrayAccesNode::checkType(SymbolTable& symbolTable) const
{ 
    return symbolTable.getVariableType(this->name);;
}

void ArrayAssignementNode::print(int indent) const
{
    printIndent(indent);
    std::cout << "ArrayAssignementNode: \n";
    printIndent(indent+1);
    std::cout << "Name: " << this->name << std::endl;
    printIndent(indent+1);
    std::cout << "Index: " << std::endl;
    this->index->print(indent+2);
    printIndent(indent+1);
    std::cout << "Expr: " << std::endl;
    this->expr->print(indent + 2);
}
const std::string& ArrayAssignementNode::getName() const
{
    return this->name;
}
const std::unique_ptr<ASTNode>& ArrayAssignementNode::getIndex() const
{
    return this->index;
}

const std::unique_ptr<ASTNode>& ArrayAssignementNode::getExpr() const
{
    return this->expr;
}
Type ArrayAssignementNode::checkType(SymbolTable& symbolTable) const
{
    return symbolTable.getVariableType(this->name);;
}



void BlockNode::print(int indent) const
{
    printIndent(indent);
    std::cout << "BlockNode: " << std::endl;
    for (int i = 0; i < Block.size(); i++) {
        Block[i]->print(indent + 1);
    }
}
const std::vector<std::unique_ptr<ASTNode>>& BlockNode::getBlock() const
{ 
    return this->Block;
}



void LireNode::print(int indent) const
{
    printIndent(indent);
    std::cout << "LireNode: "; 
    varID->print();
}
const std::unique_ptr<IdentifierNode>& LireNode::getVarID() const
{
    return varID;
}
Type LireNode::checkType(SymbolTable& symbolTable) const
{ 
    Type t = varID->checkType(symbolTable);
    if (t != Type::STRING)
    {
        std::cerr << "[AST]: Erreur de type dans lire. Seul le type STRING est accepter";
        exit(1);
    }
    return t; 
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