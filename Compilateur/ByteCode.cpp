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

ByteCode::ByteCode() {}

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
    else if (auto if_ = dynamic_cast<const IfNode*>(node)) {
        generateExpressionBytecode(if_->getCond().get(), symbolTable);

        int elseLabel = this->bytecode.size(); // Marque l'emplacement du saut
        this->bytecode.push_back({ JUMP_IF_FALSE, std::string{std::to_string(elseLabel)} });
        generateBytecode(if_->getIfBlock().get(), symbolTable);
        

        // Ajouter un saut inconditionnel (JUMP) pour éviter le bloc else
        int endIfLabel = this->bytecode.size();
        this->bytecode.push_back({ JUMP, std::string{std::to_string(endIfLabel)} });
        this->bytecode[elseLabel].arg = std::to_string(endIfLabel);

        // Générer le bytecode pour le bloc else (si présent)
        if (if_->elseBlock != nullptr) 
        {
            this->bytecode[elseLabel].arg = std::to_string(this->bytecode.size()); // Mettre à jour le saut conditionnel
            generateBytecode(if_->getElseBlock().get(), symbolTable);
        }
        this->bytecode[endIfLabel].arg = std::to_string(this->bytecode.size());
    }
    else if (auto tantque = dynamic_cast<const TantQueNode*>(node)) 
    {
        int condLabel = this->bytecode.size();
        generateExpressionBytecode(tantque->getCond().get(), symbolTable);

        int tantqueLabel = this->bytecode.size(); // Marque l'emplacement du saut
        this->bytecode.push_back({ JUMP_IF_TRUE, std::string{std::to_string(tantqueLabel)} });

        generateBytecode(tantque->getTantQueBlock().get(), symbolTable);
        this->bytecode.push_back({ JUMP, std::string{std::to_string(condLabel)} });
        this->bytecode[tantqueLabel].arg = std::to_string(this->bytecode.size());
    }
    else if (auto pour = dynamic_cast<const PourNode*>(node))
    {
        int debutJmp = this->bytecode.size();
        generateExpressionBytecode(pour->getValDebut().get(), symbolTable);
        this->bytecode.push_back({ STORE_VAR, std::string{pour->getValInit() }, Type::ENTIER});
        int blockPourLabel = this->bytecode.size();

        generateBytecode(pour->getPourBlock().get(), symbolTable);

        this->bytecode.push_back({ PUSH_VAR, std::string{pour->getValInit()}, Type::ENTIER });
        this->bytecode.push_back({ PUSH_CONST, Value{1} });
        this->bytecode.push_back({ ADD, Type::ENTIER });
        this->bytecode.push_back({ STORE_VAR, std::string{pour->getValInit() }, Type::ENTIER });
        generateExpressionBytecode(pour->getValFin().get(), symbolTable);
        this->bytecode.push_back({ PUSH_VAR, std::string{pour->getValInit()}, Type::ENTIER });
        this->bytecode.push_back({ EQUAL, Type::ENTIER });
        this->bytecode.push_back({ JUMP_IF_FALSE, std::string{std::to_string(blockPourLabel)} });
        // Rajouter +1 a la valeur de fin car de 0 a 10 va de 0 a 9
    }
    else if (auto arrayDecl = dynamic_cast<const ArrayDeclarationNode*>(node)) {
        int size = arrayDecl->getElems().size();

        generateExpressionBytecode(arrayDecl->getIndex().get(), symbolTable);
        this->bytecode.push_back({ NEW_ARRAY, std::string{arrayDecl->name}, arrayDecl->checkType(symbolTable) });

        for (int i = 0; i < size; i++)
        {
            this->bytecode.push_back({ PUSH_CONST, Value{i} });
            generateExpressionBytecode(arrayDecl->getElems()[i].get(), symbolTable);
            this->bytecode.push_back({ STORE_ARRAY, std::string{arrayDecl->name}, arrayDecl->checkType(symbolTable) });
        }

        // this->bytecode.push_back({ PUSH_ARRAY, std::string{std::to_string(size)}, arrayDecl->checkType(symbolTable) });
    }
    else if (auto arrayAssignement = dynamic_cast<const ArrayAssignementNode*>(node)) 
    {
        generateExpressionBytecode(arrayAssignement->getIndex().get(), symbolTable);
        generateExpressionBytecode(arrayAssignement->getExpr().get(), symbolTable);
        this->bytecode.push_back({ STORE_ARRAY, std::string{arrayAssignement->getName()}, arrayAssignement->checkType(symbolTable)});
    }
    else if (auto lire = dynamic_cast<const LireNode*>(node))
    {
        this->bytecode.push_back({ INPUT });
        this->bytecode.push_back({ STORE_VAR, std::string{lire->getVarID()->getName()}, lire->checkType(symbolTable)});
    }
    else if (auto block = dynamic_cast<const BlockNode*>(node)) 
    {
        for (int i = 0; i < block->getBlock().size(); i++)
        {
            generateBytecode(block->getBlock()[i].get(), symbolTable);
        }
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
    else if (auto arrayAccess = dynamic_cast<const ArrayAccesNode*>(node)) {
        generateExpressionBytecode(arrayAccess->getIndex().get(), symbolTable);
        this->bytecode.push_back({ LOAD_ARRAY, std::string{arrayAccess->getName()}, arrayAccess->checkType(symbolTable) });
    }
    else if (auto str = dynamic_cast<const StringNode*>(node)) {
        this->bytecode.push_back({ PUSH_CONST, Value{str->getValue()} });
    }
    else if (auto ident = dynamic_cast<const IdentifierNode*>(node)) {
        this->bytecode.push_back({ PUSH_VAR, std::string{ident->getName()}, ident->checkType(symbolTable) });
  
    }
    else if (auto binaryOp = dynamic_cast<const BinaryOpNode*>(node)) {
        Type checktype = binaryOp->checkType(symbolTable);
        Type operandType = binaryOp->getLeft()->checkType(symbolTable);

        generateExpressionBytecode(binaryOp->getLeft().get(), symbolTable);
        generateExpressionBytecode(binaryOp->getRight().get(), symbolTable);
        if (binaryOp->getOp() == "+") 
        {
            this->bytecode.push_back({ ADD, checktype });
        }
        else if (binaryOp->getOp() == "ET")
        {
            this->bytecode.push_back({ AND, checktype });
        }
        else if (binaryOp->getOp() == "OU")
        {
            this->bytecode.push_back({ OR, checktype });
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
        else if (binaryOp->getOp() == ".")
        {
            this->bytecode.push_back({ ADD, checktype });
        }
        else if (binaryOp->getOp() == "<")
        {
            this->bytecode.push_back({ LESS, operandType });
        }
        else if (binaryOp->getOp() == "<=")
        {
            this->bytecode.push_back({ LESS_EQUAL, operandType });
        }
        else if (binaryOp->getOp() == ">")
        {
            this->bytecode.push_back({ GREATER, operandType });
        }
        else if (binaryOp->getOp() == ">=")
        {
            this->bytecode.push_back({ GREATER_EQUAL, operandType });
        }
        else if (binaryOp->getOp() == "!=")
        {
            this->bytecode.push_back({ NOT_EQUAL, operandType });
        }
        else if (binaryOp->getOp() == "==")
        {
            this->bytecode.push_back({ EQUAL, operandType });
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
        std::cout << "[" << i << "] ";
        switch (bytecode[i].opcode) {
        case PUSH_CONST:
            std::cout << "PUSH_CONST ";
            bytecode[i].value.print();
            break;
        case PUSH_VAR:
            std::cout << "PUSH_VAR " << bytecode[i].arg << "\n";
            break;
        case AND:
            std::cout << "AND" << "\n";
            break;
        case OR:
            std::cout << "OR" << "\n";
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
        case LESS:
            std::cout << "LESS " << "\n";
            break;
        case LESS_EQUAL:
            std::cout << "LESS_EQUAL" << "\n";
            break;
        case GREATER:
            std::cout << "GREATER" << "\n";
            break;
        case GREATER_EQUAL:
            std::cout << "GREATER_EQUAL" << "\n";
            break;
        case NOT_EQUAL:
            std::cout << "NOT_EQUAL" << "\n";
            break;
        case EQUAL:
            std::cout << "EQUAL" << "\n";
            break;

        case JUMP_IF_FALSE:
            std::cout << "JUMP_IF_FALSE " << bytecode[i].arg << "\n";
            break;
        case JUMP_IF_TRUE:
            std::cout << "JUMP_IF_TRUE " << bytecode[i].arg << "\n";
            break;
        case JUMP:
            std::cout << "JUMP " << bytecode[i].arg << "\n";
            break;
        case LOAD_ARRAY:
            std::cout << "LOAD_ARRAY " << bytecode[i].arg << "\n";
            break;
        case STORE_ARRAY:
            std::cout << "STORE_ARRAY " << bytecode[i].arg << "\n";
            break;
        case NEW_ARRAY:
            std::cout << "NEW_ARRAY " << bytecode[i].arg << "\n";
            break;
        case INPUT:
            std::cout << "INPUT " << bytecode[i].arg << "\n";
            break;
        case ENDIF:
            std::cout << "ENDIF " << "\n";
            break;
        }
    }
}

void ByteCode::printOneByteCode(Instruction opcode)
{

    switch (opcode.opcode) 
{
        case PUSH_CONST:
            std::cout << "PUSH_CONST ";
            opcode.value.print();
            break;
        case PUSH_VAR:
            std::cout << "PUSH_VAR " << opcode.arg << "\n";
            break;
        case STORE_VAR:
            std::cout << "STORE_VAR " << opcode.arg << "\n";
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
        case LESS:
            std::cout << "LESS " << "\n";
            break;
        case LESS_EQUAL:
            std::cout << "LESS_EQUAL" << "\n";
            break;
        case GREATER:
            std::cout << "GREATER" << "\n";
            break;
        case GREATER_EQUAL:
            std::cout << "GREATER_EQUAL" << "\n";
            break;
        case NOT_EQUAL:
            std::cout << "NOT_EQUAL" << "\n";
            break;
        case EQUAL:
            std::cout << "EQUAL" << "\n";
            break;

        case JUMP_IF_FALSE:
            std::cout << "JUMP_IF_FALSE " << opcode.arg << "\n";
            break;
        case JUMP_IF_TRUE:
            std::cout << "JUMP_IF_TRUE " << opcode.arg << "\n";
            break;
        case JUMP:
            std::cout << "JUMP " << opcode.arg << "\n";
            break;
        case LOAD_ARRAY:
            std::cout << "LOAD_ARRAY " << opcode.arg << "\n";
            break;
        case STORE_ARRAY:
            std::cout << "STORE_ARRAY " << opcode.arg << "\n";
            break;
        case NEW_ARRAY:
            std::cout << "NEW_ARRAY " << opcode.arg << "\n";
            break;
        case ENDIF:
            std::cout << "ENDIF " << "\n";
            break;
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
            if (std::holds_alternative<int>(this->bytecode[i].value.data)) { pushStackInt(std::get<int>(this->bytecode[i].value.data)); }
            else if (std::holds_alternative<float>(this->bytecode[i].value.data)) { pushStackFloat(std::get<float>(this->bytecode[i].value.data)); }
            else if (std::holds_alternative<bool>(this->bytecode[i].value.data)) { pushStackBool(std::get<bool>(this->bytecode[i].value.data)); }
            else if (std::holds_alternative<std::string>(this->bytecode[i].value.data)) { pushStackString(std::get<std::string>(this->bytecode[i].value.data)); }
            else { std::cerr << "[EXEC BYTECODE] ERR: PUSH_CONST " << this->bytecode[i].arg << " Type inconnu" << std::endl; }
            break;
        case PUSH_VAR:
            if (this->bytecode[i].type == Type::ENTIER) { pushStackInt(this->varIntTable[this->bytecode[i].arg]); }
            else if (this->bytecode[i].type == Type::REEL) { pushStackFloat(this->varReelTable[this->bytecode[i].arg]); }
            else if (this->bytecode[i].type == Type::BOOL) { pushStackBool(this->varBoolTable[this->bytecode[i].arg]); }
            else if (this->bytecode[i].type == Type::STRING) { pushStackString(this->varStrTable[this->bytecode[i].arg]); }
            else { std::cerr << "[EXEC BYTECODE] ERR: PUSH_VAR " << this->bytecode[i].arg << " Type inconnu" << std::endl; }
            break;
        case STORE_VAR:
            if (this->bytecode[i].type == Type::ENTIER) { this->varIntTable[this->bytecode[i].arg] = popStackInt(); }
            else if (this->bytecode[i].type == Type::REEL) { this->varReelTable[this->bytecode[i].arg] = popStackReel(); }
            else if (this->bytecode[i].type == Type::BOOL) { this->varBoolTable[this->bytecode[i].arg] = popStackBool(); }
            else if (this->bytecode[i].type == Type::STRING) { this->varStrTable[this->bytecode[i].arg] = popStackString(); }
            else { std::cerr << "[EXEC BYTECODE] ERR: STORE_VAR " << this->bytecode[i].arg << " Type inconnu" << std::endl; }
            break;
        case LOAD_ARRAY:
            if (this->bytecode[i].type == Type::ENTIER)
            {
                pushStackInt(this->varArrayIntTable[this->bytecode[i].arg][popStackInt()]);
            }
            if (this->bytecode[i].type == Type::REEL)
            {
                pushStackFloat(this->varArrayFloatTable[this->bytecode[i].arg][popStackInt()]);
            }
            if (this->bytecode[i].type == Type::STRING)
            {
                pushStackString(this->varArrayStrTable[this->bytecode[i].arg][popStackInt()]);
            }
            if (this->bytecode[i].type == Type::BOOL)
            {
                pushStackInt(this->varArrayBoolTable[this->bytecode[i].arg][popStackInt()]);
            }
            break;
        case STORE_ARRAY:
            if (this->bytecode[i].type == Type::ENTIER)
            {
                this->varArrayIntTable[this->bytecode[i].arg].set_at(popStackInt(), popStackInt());
            }
            if (this->bytecode[i].type == Type::REEL)
            {
                this->varArrayFloatTable[this->bytecode[i].arg].set_at(popStackInt(), popStackReel());
            }
            if (this->bytecode[i].type == Type::STRING)
            {
                this->varArrayStrTable[this->bytecode[i].arg].set_at(popStackInt(), popStackString());
            }
            if (this->bytecode[i].type == Type::BOOL)
            {
                this->varArrayBoolTable[this->bytecode[i].arg].set_at(popStackInt(), popStackBool());
            }
            break;
        case NEW_ARRAY:
            if (this->bytecode[i].type == Type::ENTIER)
            {
                // Insérer un FixedSizeVector avec une taille max de popStackInt pour la clé array_name
                this->varArrayIntTable.emplace(this->bytecode[i].arg, FixedSizeVector<int>());
                this->varArrayIntTable[this->bytecode[i].arg].set_size(popStackInt());
            }
            if (this->bytecode[i].type == Type::REEL)
            {
                // Insérer un FixedSizeVector avec une taille max de popStackInt pour la clé array_name
                this->varArrayFloatTable.emplace(this->bytecode[i].arg, FixedSizeVector<float>());
                this->varArrayFloatTable[this->bytecode[i].arg].set_size(popStackInt());
            }
            if (this->bytecode[i].type == Type::STRING)
            {
                // Insérer un FixedSizeVector avec une taille max de popStackInt pour la clé array_name
                this->varArrayStrTable.emplace(this->bytecode[i].arg, FixedSizeVector<std::string>());
                this->varArrayStrTable[this->bytecode[i].arg].set_size(popStackInt());
            }
            if (this->bytecode[i].type == Type::BOOL)
            {
                // Insérer un FixedSizeVector avec une taille max de popStackInt pour la clé array_name
                this->varArrayBoolTable.emplace(this->bytecode[i].arg, FixedSizeVector<bool>());
                this->varArrayBoolTable[this->bytecode[i].arg].set_size(popStackInt());
            }
            break;
        case AND:
            if (this->bytecode[i].type == Type::BOOL)
            {
                bool nbBool = popStackBool();
                bool nbBool2 = popStackBool();
                pushStackBool(nbBool && nbBool2);
            }
            else { std::cerr << "[EXEC BYTECODE] ERR: ADD " << this->bytecode[i].arg << " Type non pris en charge" << std::endl; }
            break;
        case OR:
            if (this->bytecode[i].type == Type::BOOL)
            {
                bool nbBool = popStackBool();
                bool nbBool2 = popStackBool();
                pushStackBool(nbBool || nbBool2);
            }
            else { std::cerr << "[EXEC BYTECODE] ERR: ADD " << this->bytecode[i].arg << " Type non pris en charge" << std::endl; }
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
                pushStackString(nbStr2 + nbStr1);
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
        case LESS:
            if (this->bytecode[i].type == Type::ENTIER)
            {
                int rightInt = popStackInt();
                int leftInt = popStackInt();
                pushStackBool(leftInt < rightInt);
            }
            else if (this->bytecode[i].type == Type::REEL)
            {
                float rightReel = popStackReel();
                float leftReel = popStackReel();
                pushStackBool(leftReel < rightReel);
            }
            else if (this->bytecode[i].type == Type::BOOL)
            {
                bool rightBool = popStackBool();
                bool leftBool = popStackBool();
                pushStackBool(leftBool < rightBool);
            }
            else { std::cerr << "[EXEC BYTECODE] ERR: Type incompatible avec cette opération" << std::endl; exit(1); }
            break;
        case LESS_EQUAL:
            if (this->bytecode[i].type == Type::ENTIER)
            {
                int rightInt = popStackInt();
                int leftInt = popStackInt();
                pushStackBool(leftInt <= rightInt);
            }
            else if (this->bytecode[i].type == Type::REEL)
            {
                float rightReel = popStackReel();
                float leftReel = popStackReel();
                pushStackBool(leftReel <= rightReel);
            }
            else if (this->bytecode[i].type == Type::BOOL)
            {
                bool rightBool = popStackBool();
                bool leftBool = popStackBool();
                pushStackBool(leftBool <= rightBool);
            }
            else { std::cerr << "[EXEC BYTECODE] ERR: Type incompatible avec cette opération" << std::endl; exit(1); }
            break;
        case GREATER:
            if (this->bytecode[i].type == Type::ENTIER)
            {
                int rightInt = popStackInt();
                int leftInt = popStackInt();
                pushStackBool(leftInt > rightInt);
            }
            else if (this->bytecode[i].type == Type::REEL)
            {
                float rightReel = popStackReel();
                float leftReel = popStackReel();
                pushStackBool(leftReel > rightReel);
            }
            else if (this->bytecode[i].type == Type::BOOL)
            {
                bool rightBool = popStackBool();
                bool leftBool = popStackBool();
                pushStackBool(leftBool > rightBool);
            }
            else { std::cerr << "[EXEC BYTECODE] ERR: Type incompatible avec cette opération" << std::endl; exit(1); }
            break;
        case GREATER_EQUAL:
            if (this->bytecode[i].type == Type::ENTIER)
            {
                int rightInt = popStackInt();
                int leftInt = popStackInt();
                pushStackBool(leftInt >= rightInt);
            }
            else if (this->bytecode[i].type == Type::REEL)
            {
                float rightReel = popStackReel();
                float leftReel = popStackReel();
                pushStackBool(leftReel >= rightReel);
            }
            else if (this->bytecode[i].type == Type::BOOL)
            {
                bool rightBool = popStackBool();
                bool leftBool = popStackBool();
                pushStackBool(leftBool >= rightBool);
            }
            else { std::cerr << "[EXEC BYTECODE] ERR: Type incompatible avec cette opération" << std::endl; exit(1); }
            break;

        case NOT_EQUAL:
            if (this->bytecode[i].type == Type::ENTIER)
            {
                int rightInt = popStackInt();
                int leftInt = popStackInt();
                pushStackBool(leftInt != rightInt);
            }
            else if (this->bytecode[i].type == Type::REEL)
            {
                float rightReel = popStackReel();
                float leftReel = popStackReel();
                pushStackBool(leftReel != rightReel);
            }
            else if (this->bytecode[i].type == Type::BOOL)
            {
                bool rightBool = popStackBool();
                bool leftBool = popStackBool();
                pushStackBool(leftBool != rightBool);
            }
            else if (this->bytecode[i].type == Type::STRING)
            {
                std::string rightStr = popStackString();
                std::string leftStr = popStackString();
                pushStackBool(leftStr != rightStr);
            }
            else { std::cerr << "[EXEC BYTECODE] ERR: Type incompatible avec cette opération" << std::endl; exit(1); }
            break;
        case EQUAL:
            if (this->bytecode[i].type == Type::ENTIER)
            {
                int rightInt = popStackInt();
                int leftInt = popStackInt();
                pushStackBool(leftInt == rightInt);
            }
            else if (this->bytecode[i].type == Type::REEL)
            {
                float rightReel = popStackReel();
                float leftReel = popStackReel();
                pushStackBool(leftReel == rightReel);
            }
            else if (this->bytecode[i].type == Type::BOOL)
            {
                bool rightBool = popStackBool();
                bool leftBool = popStackBool();
                pushStackBool(leftBool == rightBool);
            }
            else if (this->bytecode[i].type == Type::STRING)
            {
                std::string rightStr = popStackString();
                std::string leftStr = popStackString();
                pushStackBool(leftStr == rightStr);
            }
            else { std::cerr << "[EXEC BYTECODE] ERR: Type incompatible avec cette opération" << std::endl; exit(1); }
            break;

        case JUMP_IF_FALSE:
            if (!popStackBool())
            {
                i = std::stoi(bytecode[i].arg) - 1;
                break;
            }
            break;
        case JUMP_IF_TRUE:
            if (popStackBool())
            {
                i = std::stoi(bytecode[i].arg) - 1;
                break;
            }
            break;
        case JUMP:
            i = std::stoi(bytecode[i].arg) - 1;
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
        case INPUT:
            std::string temp;
            std::cin >> temp;
            pushStackString(temp);
            break;
        }
    }

}