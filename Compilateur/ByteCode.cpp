#include "ByteCode.hpp"

/*
1: PUSH_CONST
    Args: String (name)
    Desc: Place un nombre dans la pile
2: PUSH_VAR:
    Args: String (name)
    Desc: Place la valeur d'une variable dans la pile
3: STORE_VAR
    Args: String (name)
    Desc: Pop la pile et place le chiffre dans la map des variables en fonction du nom
4: ADD
    Args: -
    Desc: Pop la pile 2 fois et additionne les deux chiffres puis push le résultat
5: PRINT
    Args: -
    Desc: Print


*/


ByteCode::ByteCode()
{

}

void ByteCode::generateBytecode(const ASTNode* node) {
    if (auto varDecl = dynamic_cast<const VarDeclarationNode*>(node)) {
        generateExpressionBytecode(varDecl->getExpr().get());
        this->bytecode.push_back({ STORE_VAR, varDecl->getName(), 0});
    }
    else if (auto assign = dynamic_cast<const AssignmentNode*>(node)) {
        generateExpressionBytecode(assign->getExpr().get());
        this->bytecode.push_back({ STORE_VAR, assign->getName(), 0});
    }
    else if (auto print = dynamic_cast<const PrintNode*>(node)) {
        generateExpressionBytecode(print->getExpr().get());
        this->bytecode.push_back({ PRINT, "", 0});
    }
    else {
        std::cerr << "erreur dans le noeud" << std::endl;
    }
}

void ByteCode::generateExpressionBytecode(const ASTNode* node) {
    if (auto number = dynamic_cast<const NumberNode*>(node)) {
        this->bytecode.push_back({ PUSH_CONST, "", std::stoi(number->getValue())});
    }
    else if (auto ident = dynamic_cast<const IdentifierNode*>(node)) {
        this->bytecode.push_back({ PUSH_VAR, ident->getName(), 0});
    }
    else if (auto binaryOp = dynamic_cast<const BinaryOpNode*>(node)) {
        generateExpressionBytecode(binaryOp->getLeft().get());
        generateExpressionBytecode(binaryOp->getRight().get());
        if (binaryOp->getOp() == "+") 
        {
            this->bytecode.push_back({ ADD });
        }
        else if (binaryOp->getOp() == "-") 
        {
            this->bytecode.push_back({ SUB });
        }
        else if (binaryOp->getOp() == "*") 
        {
            this->bytecode.push_back({ MULT });
        }
        else if (binaryOp->getOp() == "/")
        {
            this->bytecode.push_back({ DIV });
        }
    }
    else {
        throw std::runtime_error("Expression non supportée.");
    }
}

void ByteCode::generateAllByteCode(const std::vector<std::unique_ptr<ASTNode>>& AST)
{
    for (int i = 0; i < AST.size(); i++)
    {
        generateBytecode(AST[i].get());
    }
    std::clog << "[BYTECODE] Analyse OK" << std::endl;
}

void ByteCode::printByteCode()
{
    for (int i = 0; i < this->bytecode.size() ; i++)
    {
        switch (bytecode[i].opcode) {
        case PUSH_CONST:
            std::cout << "PUSH_CONST " << bytecode[i].value << "\n";
            break;
        case PUSH_VAR:
            std::cout << "PUSH_VAR " << bytecode[i].arg << "\n";
            break;
        case STORE_VAR:
            std::cout << "STORE_VAR " << bytecode[i].arg << "\n";
            break;
        case ADD:
            std::cout << "ADD\n";
            break;
        case PRINT:
            std::cout << "PRINT " << "\n";
            break;
        case SUB:
            std::cout << "SUB " << "\n";
            break;
        case MULT:
            std::cout << "MULT " << "\n";
            break;
        case DIV:
            std::cout << "DIV " << "\n";
            break;
        }
    }
}




void ByteCode::pushStackInt(int value)
{
    this->stackInt.push_back(value);
}


int ByteCode::popStackInt()
{
    if (!this->stackInt.empty())
    {

        int last = this->stackInt.back();
        this->stackInt.pop_back();
        return last;
    }
    else
    {
        std::cerr << "[EXEC BYTECODE] ERR: La pile est vide" << std::endl;
        exit(1);
    }
}

void ByteCode::executeByteCode()
{

    for (int i = 0; i < this->bytecode.size(); i++)
    {
        int nb1;
        int nb2;
        int nb;
        switch (bytecode[i].opcode) {
        case PUSH_CONST:
            pushStackInt(this->bytecode[i].value);
            break;
        case PUSH_VAR:
            pushStackInt(this->varIntTable[this->bytecode[i].arg]);
            break;
        case STORE_VAR:
            this->varIntTable[this->bytecode[i].arg] = popStackInt();
            break;
        case ADD:
            nb1 = popStackInt();
            nb2 = popStackInt();
            pushStackInt(nb1 + nb2);
            break;
        case SUB:
            nb1 = popStackInt();
            nb2 = popStackInt();
            pushStackInt(nb1 - nb2);
            break;
        case MULT:
            nb1 = popStackInt();
            nb2 = popStackInt();
            pushStackInt(nb1 * nb2);
            break;
        case DIV:
            nb1 = popStackInt();
            nb2 = popStackInt();
            pushStackInt(nb1 / nb2);
            break;
        case PRINT:
            nb = popStackInt();
            std::cout << nb << std::endl;
            break;
        }
    }

}