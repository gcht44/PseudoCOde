#ifndef LEXER_HPP
#define LEXER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <sstream>

enum class TokenType {
    // Tokens existants
    NAME,
    NUMBER,
    STRING,
    TRUE,
    FALSE,
    // Opérateurs
    PLUS, MINUS, MULTIPLY, DIVIDE,
    // Comparaisons
    EQUAL, NOT_EQUAL, LESS, LESS_EQUAL, GREATER, GREATER_EQUAL, EQUAL_EQUAL,
    // Parenthèses, crochets, etc.
    LPAREN, RPAREN, LBRACKET, RBRACKET,
    // Ponctuation
    COMMA, COLON, SEMICOLON, DOT,
    // Mots-clés
    IF, ELSE, INT, STRINGVAR, FLOAT, BOOL, PRINT, VARIABLE, DEBUT, FIN, FINSI, TANTQUE, FINTANTQUE,
    // Indentation
    INDENT, DEDENT, NEWLINE,
    // Fin de fichier
    END
};

class Token {
public:
    TokenType type;
    std::string value;
    int line;
    int column;

    Token(TokenType t, const std::string& v, int l, int c)
        : type(t), value(v), line(l), column(c) {}
};

class Lexer {
public:
    Lexer(const std::string& source) : contenu(source), pos(0), nbLigne(1), currentIndent(0) {
        indentStack.push(0); // Niveau d'indentation initial
    }

    std::vector<Token> Tokenise();
    void printTokens(std::vector<Token> tokenList);

private:
    std::string contenu;
    size_t pos;
    int nbLigne;
    int currentIndent;
    std::stack<int> indentStack;
    const int INDENT_SIZE = 4; // Taille d'une unité d'indentation (2 espaces)

    Token GetNextToken();
    char CurrentChar() const;
    char NextChar();
    bool IsEOF() const;
    void SkipWhitespace();
    bool isVide(const std::string& line) const;

    // Méthodes pour l'indentation
    int calculateIndentation(const std::string& line) const;
    std::vector<Token> handleIndentation(const std::string& line);

    // Méthodes pour les différents types de tokens
    Token ProcessIdentifier();
    Token ProcessNumber();
    Token ProcessString();

};

#endif