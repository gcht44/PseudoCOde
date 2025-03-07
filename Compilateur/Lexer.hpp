#ifndef LEXER_HPP
#define LEXER_HPP

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <cctype>

enum class TokenType {
    KEYWORD,
    IDENTIFIER,
    EQUALS,
    NUMBER,
    SEMICOLON,
    LPAREN,
    RPAREN,
    PLUS,
    SUB,
    MULT,
    DIV,
    COLON,
    DOT,
    END
};

struct Token {
    TokenType type;
    std::string value;
    int li;
    int col;
    Token(TokenType type, std::string value, int li, int col) : type(type), value(std::move(value)), li(li), col(col) {}
};


class Lexer
{

public:
    Lexer(std::string f); 
    // bool loadFile(std::string f);
    std::vector<Token> Tokenise();
    void printTokens(std::vector<Token> t); // Debug function

private:
    std::vector<std::string> contenu;
    // std::vector<Token> TokenList;
    int pos;
    int nbLigne;

    bool isVide(const std::string& ligne);
    Token readIdentifierOrKeyword(std::string ligne);
    Token readNumber(std::string ligne);
    Token GetNextToken(std::string ligne);

};

#endif