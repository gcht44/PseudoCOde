#include "Parser.hpp"
#include <iostream>

Parser::Parser(std::vector<Token> tokens) : TokenList(std::move(tokens)), pos(0), isVarParse(false), currentIndent(0) {
    this->programAST = std::make_unique<ProgramNode>();;
}

bool Parser::parseProg() {

    while (pos < TokenList.size() && TokenList[pos].type != TokenType::END) {
        // Ignorer les sauts de ligne au niveau racine
        if (TokenList[pos].type == TokenType::NEWLINE) {
            pos++;
            continue;
        }
        if (match(TokenType::VARIABLE))
        {
            if (!match(TokenType::COLON)) { err("':' Attendu après 'VARIABlE'"); }
            if (!match(TokenType::NEWLINE)) { err("Saut de ligne attendu après 'VARIABlE'"); }
            std::unique_ptr<BlockNode> varDecl = parseBlock();
            for (int i = 0; i < varDecl->getBlock().size(); i++)
            {
                if (auto varDeclVerif = dynamic_cast<const VarDeclarationNode*>(varDecl->getBlock()[i].get()))
                {
                    continue;
                }
                err("Vous pouvez seulement declarer des variables ici");
            }
            // Ajouter une manière de verifier qu'ily a seulement des declarations de variable
            programAST->addStatement(std::move(varDecl));
        }
        if (match(TokenType::DEBUT))
        {
            if (!match(TokenType::COLON)) { err("':' Attendu après 'DEBUT'"); }
            if (!match(TokenType::NEWLINE)) { err("Saut de ligne attendu après 'VARIABlE'"); }
            auto debut = parseBlock();
            for (int i = 0; i < debut->getBlock().size(); i++)
            {
                auto varDeclVerif = dynamic_cast<const VarDeclarationNode*>(debut->getBlock()[i].get());
                if (varDeclVerif == nullptr)
                {
                    continue;
                }
                err("Vous pouvez pas declarer de variable ici");
            }
            if (!match(TokenType::FIN)) { err("Attendu: Mot clé FIN"); }
            pos++;
            // Ajouter une manière de verifier qu'ily n'a pas des declarations de variable
            programAST->addStatement(std::move(debut));
        }

    }
    return true;
}

std::unique_ptr<ProgramNode>& Parser::getAST() {
    return programAST;
}

SymbolTable& Parser::getSymbolTable() {
    return symbolTable;
}

std::unique_ptr<ASTNode> Parser::parseStatement() {
    if (pos >= TokenList.size()) {
        err("Fin de fichier inattendue");
        return nullptr;
    }

    TokenType type = TokenList[pos].type;


    if (type == TokenType::NAME) 
    {
        return parseAssignement();
    }
    else if (type == TokenType::PRINT) 
    {
        return parsePrint();
    }
    else if (type == TokenType::INT || type == TokenType::FLOAT || type == TokenType::BOOL || type == TokenType::STRINGVAR)
    {
        return parseVar();
    }
    else if (type == TokenType::IF) 
    {
        return parseIf();
    }
    else if (type == TokenType::TANTQUE)
    {
        return parseTantQue();
    }
    else if (type == TokenType::POUR)
    {
        return parsePour();
    }
    else 
    {
        err("Instruction non reconnue: " + TokenList[pos].value);
        return nullptr;
    }
}

std::unique_ptr<BlockNode> Parser::parseBlock() {
    std::vector<std::unique_ptr<ASTNode>> statements;

    // Un bloc commence par un INDENT
    if (!match(TokenType::INDENT)) {
        err("Attendu: début de bloc (indentation)");
        return nullptr;
    }

    // Analyser les instructions jusqu'à rencontrer un DEDENT
    while (pos < TokenList.size() && TokenList[pos].type != TokenType::DEDENT) {
        // Ignorer les sauts de ligne à l'intérieur du bloc
        if (TokenList[pos].type == TokenType::NEWLINE) {
            pos++;
            continue;
        }

        auto stmt = parseStatement();
        if (stmt) {
            statements.push_back(std::move(stmt));
        }
        else {
            // En cas d'erreur, essayer de récupérer jusqu'à la prochaine instruction
            while (pos < TokenList.size() &&
                TokenList[pos].type != TokenType::NEWLINE &&
                TokenList[pos].type != TokenType::DEDENT) {
                pos++;
            }
            if (pos < TokenList.size() && TokenList[pos].type == TokenType::NEWLINE) {
                pos++; // Sauter le NEWLINE
            }
        }
    }

    // Un bloc se termine par un DEDENT
    if (!match(TokenType::DEDENT)) {
        err("Attendu: fin de bloc (désindentation)");
        // On continue quand même pour la récupération d'erreur
    }

    return std::make_unique<BlockNode>(std::move(statements));
}

std::unique_ptr<ASTNode> Parser::parseIf() {
    if (!match(TokenType::IF)) {
        err("Attendu: 'if'");
        return nullptr;
    }

    auto condition = parseExpressionAST();
    if (!condition) {
        err("Expression de condition invalide");
        return nullptr;
    }

    if (!match(TokenType::COLON)) 
    {
        err("Attendu: ':'");
        // On continue quand même
    }

    // Après le 'then', on s'attend à un NEWLINE
    if (!match(TokenType::NEWLINE)) {
        err("Attendu: nouvelle ligne après 'then'");
        // On continue quand même
    }

    auto thenBlock = parseBlock();
    if (!thenBlock) {
        return nullptr;
    }

    std::unique_ptr<ASTNode> elseBlock = nullptr;
    if (pos < TokenList.size() && TokenList[pos].type == TokenType::ELSE) {
        pos++; // Consommer le 'else'
        if (!match(TokenType::COLON)) { err("Attendu: ':'"); }
        // Après le 'else', on s'attend à un NEWLINE
        if (!match(TokenType::NEWLINE)) {
            err("Attendu: nouvelle ligne après 'else'");
            // On continue quand même
        }

        elseBlock = parseBlock();
        if (!elseBlock) {
            return nullptr;
        }
        if (!match(TokenType::FINSI)) { err("Attendu: Mot clé FIN"); }
        pos++;
    }

    // Assurez-vous que le constructeur IfNode prend les bons arguments
    // et que la classe n'est pas abstraite
    return std::make_unique<IfNode>(std::move(condition), std::move(thenBlock), std::move(elseBlock));
}

std::unique_ptr<ASTNode> Parser::parseAssignement() {
    if (pos >= TokenList.size() || TokenList[pos].type != TokenType::NAME) {
        err("Attendu: nom de variable");
        return nullptr;
    }

    std::string varName = TokenList[pos].value;
    pos++; // Consommer le nom

    if (!match(TokenType::EQUAL)) {
        err("Attendu: '='");
        return nullptr;
    }

    auto expr = parseExpressionAST();
    if (!expr) {
        err("Expression invalide dans l'assignation");
        return nullptr;
    }
    if (!match(TokenType::SEMICOLON)) { err("Attendu: ';' après l'assignation"); }

    // Après l'assignation, on s'attend à un NEWLINE
    if (!match(TokenType::NEWLINE)) {
        err("Attendu: nouvelle ligne après assignation");
        // On continue quand même
    }

    // Assurez-vous que le constructeur VarDeclarationNode prend les bons arguments
    return std::make_unique<VarDeclarationNode>(varName, std::move(expr));
}

std::unique_ptr<ASTNode> Parser::parseVar() {
    std::string varName;
    std::unique_ptr<ASTNode> expr;
    if (match(TokenType::INT))
    {
        if (pos >= TokenList.size() || TokenList[pos].type != TokenType::NAME) {
            err("Attendu: nom de variable");
            return nullptr;
        }

        varName = TokenList[pos].value;
        pos++; // Consommer le nom

        if (!match(TokenType::EQUAL)) {
            err("Attendu: '='");
            return nullptr;
        }

        expr = parseExpressionAST();
        if (!expr) {
            err("Expression invalide dans l'assignation");
            return nullptr;
        }
        symbolTable.addVariable(varName, Type::ENTIER);
    }
    else if (match(TokenType::FLOAT))
    {
        if (pos >= TokenList.size() || TokenList[pos].type != TokenType::NAME) {
            err("Attendu: nom de variable");
            return nullptr;
        }

        varName = TokenList[pos].value;
        pos++; // Consommer le nom

        if (!match(TokenType::EQUAL)) {
            err("Attendu: '='");
            return nullptr;
        }

        expr = parseExpressionAST();
        if (!expr) {
            err("Expression invalide dans l'assignation");
            return nullptr;
        }
        symbolTable.addVariable(varName, Type::REEL);
    }
    else if (match(TokenType::BOOL))
    {
        if (pos >= TokenList.size() || TokenList[pos].type != TokenType::NAME) {
            err("Attendu: nom de variable");
            return nullptr;
        }

        varName = TokenList[pos].value;
        pos++; // Consommer le nom

        if (!match(TokenType::EQUAL)) {
            err("Attendu: '='");
            return nullptr;
        }

        expr = parseExpressionAST();
        if (!expr) {
            err("Expression invalide dans l'assignation");
            return nullptr;
        }
        symbolTable.addVariable(varName, Type::BOOL);
    }
    else if (match(TokenType::STRINGVAR))
    {
        if (pos >= TokenList.size() || TokenList[pos].type != TokenType::NAME) {
            err("Attendu: nom de variable");
            return nullptr;
        }

        varName = TokenList[pos].value;
        pos++; // Consommer le nom

        if (!match(TokenType::EQUAL)) {
            err("Attendu: '='");
            return nullptr;
        }

        expr = parseExpressionAST();
        if (!expr) {
            err("Expression invalide dans l'assignation");
            return nullptr;
        }
        symbolTable.addVariable(varName, Type::STRING);
    }
    if (!match(TokenType::SEMICOLON)) { err("Attendu: ';' après la ligne"); }
    if (!match(TokenType::NEWLINE)) {
        err("Attendu: nouvelle ligne après assignation");
        // On continue quand même
    }

    // Assurez-vous que le constructeur VarDeclarationNode prend les bons arguments
    return std::make_unique<VarDeclarationNode>(varName, std::move(expr));
}


std::unique_ptr<ASTNode> Parser::parsePrint() {
    if (!match(TokenType::PRINT)) {
        err("Attendu: 'ecrire'");
        return nullptr;
    }

    auto expr = parseExpressionAST();
    if (!expr) {
        err("Expression invalide dans print");
        return nullptr;
    }
    if (!match(TokenType::SEMICOLON)) { err("Attendu: ';' à la fin de la ligne"); }
    if (!match(TokenType::NEWLINE)) {
        err("Attendu: nouvelle ligne après print");
        // On continue quand même
    }

    return std::make_unique<PrintNode>(std::move(expr));
}

std::unique_ptr<ASTNode> Parser::parseExpressionAST() {
    auto LHS = parseTermAST();
    if (!LHS) {
        return nullptr;
    }

    return parseBinOpRHS(0, std::move(LHS));
}

std::unique_ptr<ASTNode> Parser::parseTantQue()
{
    if (!match(TokenType::TANTQUE)) { err("Attendu: mot clé TANTQUE"); }

    auto condition = parseExpressionAST();
    if (!condition) {
        err("Expression de condition invalide");
        return nullptr;
    }
    
    if (!match(TokenType::COLON)) { err("Attendu: ':' (" + tokenTypeToStr(TokenList[pos].type) + " value: " + TokenList[pos].value + ")"); }
    if (!match(TokenType::NEWLINE)) { err("Attendu: nouvelle ligne après 'then'"); }

    auto tantqueBlock = parseBlock();
    if (!tantqueBlock) {
        return nullptr;
    }
    if (!match(TokenType::FINTANTQUE)) { err("Attendu: Mot clé FIN"); }
    pos++;

    return std::make_unique<TantQueNode>(std::move(condition), std::move(tantqueBlock));
}

std::unique_ptr<ASTNode> Parser::parsePour()
{
    std::unique_ptr<ASTNode> numOrName1 = nullptr;
    std::unique_ptr<ASTNode> numOrName2 = nullptr;

    if (!match(TokenType::POUR)) { err("Attendu: mot clé POUR"); }
    if (!match(TokenType::NAME)) { err("Attendu: variable"); }
    std::string var = TokenList[pos - 1].value;
    if (!match(TokenType::DE)) { err("Attendu: mot clé 'DE'"); }


    if (match(TokenType::NUMBER)) { numOrName1 = std::make_unique<IntNode>(TokenList[pos - 1].value); }
    else if (match(TokenType::NAME)) { numOrName1 = std::make_unique<IdentifierNode>(TokenList[pos - 1].value); }
    else { err("Attendu: NUMBER ou variable"); }

    if (!match(TokenType::A)) { err("Attendu: mot clé 'A'"); }

    if (match(TokenType::NUMBER)) { numOrName2 = std::make_unique<IntNode>(TokenList[pos - 1].value); }
    else if (match(TokenType::NAME)) { numOrName2 = std::make_unique<IdentifierNode>(TokenList[pos - 1].value); }
    else { err("Attendu: NUMBER ou variable"); }

    if (!match(TokenType::COLON)) { err("Attendu: ':' (" + tokenTypeToStr(TokenList[pos].type) + " value: " + TokenList[pos].value + ")"); }
    if (!match(TokenType::NEWLINE)) { err("Attendu: nouvelle ligne après"); }

    auto pourBlock = parseBlock();
    if (!pourBlock) {
        return nullptr;
    }
    if (!match(TokenType::FINPOUR)) { err("Attendu: Mot clé FIN"); }
    pos++;

   return std::make_unique<PourNode>(std::move(var), std::move(numOrName1), std::move(numOrName2), std::move(pourBlock));
}

std::unique_ptr<ASTNode> Parser::parseBinOpRHS(int ExprPrec, std::unique_ptr<ASTNode> LHS) {
    while (true) {
        int TokPrec = getTokenPrecedence();

        if (TokPrec < ExprPrec) {
            return LHS;
        }

        std::string BinOp = TokenList[pos].value;
        pos++; // Consommer l'opérateur

        auto RHS = parseTermAST();
        if (!RHS) {
            return nullptr;
        }

        int NextPrec = getTokenPrecedence();
        if (TokPrec < NextPrec) {
            RHS = parseBinOpRHS(TokPrec + 1, std::move(RHS));
            if (!RHS) {
                return nullptr;
            }
        }

        // Assurez-vous que le constructeur BinaryOpNode prend les bons arguments
        LHS = std::make_unique<BinaryOpNode>(BinOp, std::move(LHS), std::move(RHS));
    }
}

int Parser::getTokenPrecedence() {
    if (pos >= TokenList.size()) {
        return -1;
    }

    TokenType type = TokenList[pos].type;
    switch (type) {
    case TokenType::DOT:
        return 1;
    case TokenType::PLUS:
    case TokenType::MINUS:
        return 10;
    case TokenType::MULTIPLY:
    case TokenType::DIVIDE:
    // case TokenType::MODULO:
        return 20;
    case TokenType::EQUAL:
    case TokenType::NOT_EQUAL:
    case TokenType::EQUAL_EQUAL:
    case TokenType::LESS:
    case TokenType::LESS_EQUAL:
    case TokenType::GREATER:
    case TokenType::GREATER_EQUAL:
        return 5;
    default:
        return -1;
    }
}

std::unique_ptr<ASTNode> Parser::parseTermAST() {
    auto Primary = parseFactorAST();
    if (!Primary) {
        return nullptr;
    }

    return Primary;
}

std::unique_ptr<ASTNode> Parser::parseFactorAST() {
    TokenType type = TokenList[pos].type;

    if (type == TokenType::NAME) {
        std::string name = TokenList[pos].value;
        pos++; // Consommer le nom

        // Assurez-vous que le constructeur IdentifierNode prend les bons arguments
        return std::make_unique<IdentifierNode>(name);
    }
    else if (type == TokenType::NUMBER) {
        std::string value = TokenList[pos].value;
        pos++; // Consommer le nombre

        // Assurez-vous que le constructeur IntNode prend les bons arguments
        return std::make_unique<IntNode>(value);
    }
    else if (type == TokenType::STRING) {
        std::string value = TokenList[pos].value;
        pos++; // Consommer la chaîne

        return std::make_unique<StringNode>(value);
    }
    else if (type == TokenType::TRUE || type == TokenType::FALSE) {
        std::string value = TokenList[pos].value;
        pos++; // Consommer la chaîne

        return std::make_unique<BoolNode>(value);
    }
    else if (type == TokenType::LPAREN) {
        pos++; // Consommer '('

        auto expr = parseExpressionAST();
        if (!expr) {
            return nullptr;
        }

        if (!match(TokenType::RPAREN)) {
            err("Attendu: ')'");
            return nullptr;
        }

        return expr;
    }
    else {
        err("Expression attendue, trouvé: " + tokenTypeToStr(TokenList[pos].type));
        return nullptr;
    }
}

bool Parser::match(TokenType type) {
    if (pos < TokenList.size() && TokenList[pos].type == type) {
        pos++;
        return true;
    }
    return false;
}

bool Parser::match(TokenType type, std::string valWaiting) {
    if (pos < TokenList.size() && TokenList[pos].type == type && TokenList[pos].value == valWaiting) {
        pos++;
        return true;
    }
    return false;
}

void Parser::err(std::string msg) {
    if (pos < TokenList.size()) {
        std::cerr << "Erreur (ligne " << TokenList[pos].line << ", colonne " << TokenList[pos].column << "): "
            << msg << std::endl;
    }
    else {
        std::cerr << "Erreur: " << msg << std::endl;
    }
    exit(1);
}

std::string Parser::tokenTypeToStr(TokenType token) {
    // Implémentation simple pour convertir les types de tokens en chaînes
    switch (token) {
    case TokenType::NAME: return "identifiant";
    case TokenType::NUMBER: return "nombre";
    case TokenType::STRING: return "chaîne";
    case TokenType::PLUS: return "+";
    case TokenType::MINUS: return "-";
    case TokenType::MULTIPLY: return "*";
    case TokenType::DIVIDE: return "/";
    case TokenType::EQUAL: return "=";
    case TokenType::IF: return "if";
    // case TokenType::THEN: return "then";
    case TokenType::ELSE: return "else";
    case TokenType::PRINT: return "print";
    case TokenType::INDENT: return "indentation";
    case TokenType::DEDENT: return "désindentation";
    case TokenType::NEWLINE: return "nouvelle ligne";

    case TokenType::FIN: return "fin";
    case TokenType::FINTANTQUE: return "fin tant que";
    case TokenType::FINSI: return "fin si";

    case TokenType::TANTQUE: return "tant que";
    case TokenType::EQUAL_EQUAL: return "egal egal";
    case TokenType::NOT_EQUAL: return "pas egal";
    default: return "token inconnu";
    }
}
