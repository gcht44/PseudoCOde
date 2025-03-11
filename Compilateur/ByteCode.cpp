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

void ByteCode::generateBytecode(const ASTNode* node, SymbolTable& symbolTable) {
    if (auto varDecl = dynamic_cast<const VarDeclarationNode*>(node)) {
        generateExpressionBytecode(varDecl->getExpr().get(), symbolTable);
        this->bytecode.push_back({ STORE_VAR, std::string{varDecl->getName()}, varDecl->checkType(symbolTable) });
    }
    else if (auto assign = dynamic_cast<const AssignmentNode*>(node)) {
        generateExpressionBytecode(assign->getExpr().get(), symbolTable);
        this->bytecode.push_back({ STORE_VAR, std::string{assign->getName()}, assign->checkType(symbolTable) });
    }
    else if (auto print = dynamic_cast<const PrintNode*>(node)) {
        generateExpressionBytecode(print->getExpr().get(), symbolTable);
        this->bytecode.push_back({ PRINT, print->checkType(symbolTable) });

    }
    else {
        std::cerr << "erreur dans le noeud" << std::endl;
    }
}

void ByteCode::generateExpressionBytecode(const ASTNode* node, SymbolTable& symbolTable) {
    if (auto entier = dynamic_cast<const IntNode*>(node)) {
        this->bytecode.push_back({ PUSH_CONST, Value{std::stoi(entier->getValue())} });
    }
    else if (auto reel = dynamic_cast<const ReelNode*>(node)) {
        this->bytecode.push_back({ PUSH_CONST, Value{std::stof(reel->getValue())} });
    }
    else if (auto bool_ = dynamic_cast<const BoolNode*>(node)) {
        if (bool_->getValue() == "TRUE")
        {
            this->bytecode.push_back({ PUSH_CONST, Value{true} });
        }
        else
        {
            this->bytecode.push_back({ PUSH_CONST, Value{false} });
        }
    }
    else if (auto str = dynamic_cast<const StringNode*>(node)) {
        this->bytecode.push_back({ PUSH_CONST, Value{str->getValue()} });
    }
    else if (auto ident = dynamic_cast<const IdentifierNode*>(node)) {
        this->bytecode.push_back({ PUSH_VAR, std::string{ident->getName()}, ident->checkType(symbolTable) });
  
    }
    else if (auto binaryOp = dynamic_cast<const BinaryOpNode*>(node)) {
        Type checktype = binaryOp->checkType(symbolTable);
        generateExpressionBytecode(binaryOp->getLeft().get(), symbolTable);
        generateExpressionBytecode(binaryOp->getRight().get(), symbolTable);
        if (binaryOp->getOp() == "+") 
        {
            this->bytecode.push_back({ ADD, checktype });
        }
        else if (binaryOp->getOp() == "-") 
        {
            this->bytecode.push_back({ SUB, checktype });
        }
        else if (binaryOp->getOp() == "*") 
        {
            this->bytecode.push_back({ MULT, checktype });
        }
        else if (binaryOp->getOp() == "/")
        {
            this->bytecode.push_back({ DIV, checktype });
        }
    }
    else {
        std::cout << "Expression non supportée." << std::endl;
    }
}

void ByteCode::generateAllByteCode(const std::vector<std::unique_ptr<ASTNode>>& AST, SymbolTable& symbolTable)
{
    for (int i = 0; i < AST.size(); i++)
    {
        generateBytecode(AST[i].get(), symbolTable);
    }
    std::clog << "[BYTECODE] Analyse OK" << std::endl;
}

void ByteCode::printByteCode()
{
    for (int i = 0; i < this->bytecode.size() ; i++)
    {
        switch (bytecode[i].opcode) {
        case PUSH_CONST:
            std::cout << "PUSH_CONST ";
            bytecode[i].value.print();
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
            std::cout << "SUB" << "\n";
            break;
        case MULT:
            std::cout << "MULT" << "\n";
            break;
        case DIV:
            std::cout << "DIV" << "\n";
            break;
        }
    }
}




void ByteCode::pushStackInt(int value)
{
    this->stackInt.push_back(value);
}
void ByteCode::pushStackFloat(float value)
{
    this->stackReel.push_back(value);
}
void ByteCode::pushStackBool(bool value)
{
    this->stackBool.push_back(value);
}
void ByteCode::pushStackString(std::string value)
{
    this->stackStr.push_back(value);
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
float ByteCode::popStackReel()
{
    if (!this->stackReel.empty())
    {
        float last = this->stackReel.back();
        this->stackReel.pop_back();
        return last;
    }
    else
    {
        std::cerr << "[EXEC BYTECODE] ERR: La pile est vide" << std::endl;
        exit(1);
    }
}
bool ByteCode::popStackBool()
{
    if (!this->stackBool.empty())
    {
        bool last = this->stackBool.back();
        this->stackBool.pop_back();
        return last;
    }
    else
    {
        std::cerr << "[EXEC BYTECODE] ERR: La pile est vide" << std::endl;
        exit(1);
    }
}
std::string ByteCode::popStackString()
{
    if (!this->stackStr.empty())
    {
        std::string last = this->stackStr.back();
        this->stackStr.pop_back();
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
        switch (bytecode[i].opcode) {
        case PUSH_CONST:
            if (std::holds_alternative<int>(this->bytecode[i].value.data))              { pushStackInt(std::get<int>(this->bytecode[i].value.data));            }
            else if (std::holds_alternative<float>(this->bytecode[i].value.data))       { pushStackFloat(std::get<float>(this->bytecode[i].value.data));        }
            else if (std::holds_alternative<bool>(this->bytecode[i].value.data))        { pushStackBool(std::get<bool>(this->bytecode[i].value.data));          }
            else if (std::holds_alternative<std::string>(this->bytecode[i].value.data)) { pushStackString(std::get<std::string>(this->bytecode[i].value.data)); }
            else { std::cerr << "[EXEC BYTECODE] ERR: PUSH_CONST " << this->bytecode[i].arg << " Type inconnu" << std::endl; }
            break;
        case PUSH_VAR:
            if (this->bytecode[i].type == Type::ENTIER)      { pushStackInt(this->varIntTable[this->bytecode[i].arg]);     }
            else if (this->bytecode[i].type == Type::REEL)   { pushStackFloat(this->varReelTable[this->bytecode[i].arg]);  }
            else if (this->bytecode[i].type == Type::BOOL)   { pushStackBool(this->varBoolTable[this->bytecode[i].arg]);   }
            else if (this->bytecode[i].type == Type::STRING) { pushStackString(this->varStrTable[this->bytecode[i].arg]);  }
            else { std::cerr << "[EXEC BYTECODE] ERR: PUSH_VAR " << this->bytecode[i].arg << " Type inconnu" << std::endl; }
            break;
        case STORE_VAR:
            if (this->bytecode[i].type == Type::ENTIER)      { this->varIntTable[this->bytecode[i].arg] = popStackInt();    }
            else if (this->bytecode[i].type == Type::REEL)   { this->varReelTable[this->bytecode[i].arg] = popStackReel();  }
            else if (this->bytecode[i].type == Type::BOOL)   { this->varBoolTable[this->bytecode[i].arg] = popStackBool();  } 
            else if (this->bytecode[i].type == Type::STRING) { this->varStrTable[this->bytecode[i].arg] = popStackString(); } 
            else { std::cerr << "[EXEC BYTECODE] ERR: STORE_VAR " << this->bytecode[i].arg << " Type inconnu" << std::endl; }
            break;
        case ADD:
            if (this->bytecode[i].type == Type::ENTIER)
            {
                int nbEntier1 = popStackInt();
                int nbEntier2 = popStackInt();
                pushStackInt(nbEntier1 + nbEntier2);
            }
            else if (this->bytecode[i].type == Type::REEL)
            {
                float nbReel1 = popStackReel();
                float nbReel2 = popStackReel();
                pushStackFloat(nbReel1 + nbReel2);
            }
            else if (this->bytecode[i].type == Type::STRING)
            {
                std::string nbStr1 = popStackString();
                std::string nbStr2 = popStackString();
                pushStackString(nbStr1 + nbStr2);
            }
            else { std::cerr << "[EXEC BYTECODE] ERR: ADD " << this->bytecode[i].arg << " Type non pris en charge" << std::endl; }
            break;
        case SUB:
            if (this->bytecode[i].type == Type::ENTIER)
            {
                int nbEntier1 = popStackInt();
                int nbEntier2 = popStackInt();
                pushStackInt(nbEntier2 + nbEntier1);
            }
            else if (this->bytecode[i].type == Type::REEL)
            {
                float nbReel1 = popStackReel();
                float nbReel2 = popStackReel();
                pushStackFloat(nbReel2 - nbReel1);
            }
            else { std::cerr << "[EXEC BYTECODE] ERR: SUB " << this->bytecode[i].arg << " Type non pris en charge" << std::endl; }
            break;
        case MULT:
            if (this->bytecode[i].type == Type::ENTIER)
            {
                int nbEntier1 = popStackInt();
                int nbEntier2 = popStackInt();
                pushStackInt(nbEntier1 * nbEntier2);
            }
            else if (this->bytecode[i].type == Type::REEL)
            {
                float nbReel1 = popStackReel();
                float nbReel2 = popStackReel();
                pushStackFloat(nbReel1 * nbReel2);
            }
            else { std::cerr << "[EXEC BYTECODE] ERR: MULT " << this->bytecode[i].arg << " Type non pris en charge" << std::endl; }
            break;
        case DIV:
            if (this->bytecode[i].type == Type::ENTIER)
            {
                int nbEntier1 = popStackInt();
                int nbEntier2 = popStackInt();
                pushStackInt(nbEntier1 / nbEntier2);
            }
            else if (this->bytecode[i].type == Type::REEL)
            {
                float nbReel1 = popStackReel();
                float nbReel2 = popStackReel();
                pushStackFloat(nbReel1 / nbReel2);
            }
            else { std::cerr << "[EXEC BYTECODE] ERR: DIV " << this->bytecode[i].arg << " Type non pris en charge" << std::endl; }
            break;
        case PRINT:
            if (this->bytecode[i].type == Type::ENTIER)
            {
                int nbEntier = popStackInt();
                std::cout << nbEntier << std::endl;
            }
            else if (this->bytecode[i].type == Type::REEL)
            {
                float nbFloat = popStackReel();
                std::cout << std::fixed << std::setprecision(1);
                std::cout << nbFloat << std::endl;
            }
            else if (this->bytecode[i].type == Type::BOOL)
            {
                bool bool_ = popStackBool();
                std::cout << bool_ << std::endl;
            }
            else if (this->bytecode[i].type == Type::STRING)
            {
                std::string str = popStackString();
                std::cout << str << std::endl;
            }
            else { std::cerr << "[EXEC BYTECODE] ERR: PRINT " << this->bytecode[i].arg << " Type non pris en charge" << std::endl; }
            break;
        }
    }

}