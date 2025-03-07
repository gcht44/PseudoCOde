#ifndef PARSER_HPP
#define PARSER_HPP

#include <iostream>
#include <vector>
#include "Lexer.hpp"
#include "AST.hpp"

class Parser {

public:
	Parser(std::vector<Token> tokens);
	bool parseProg();
	std::unique_ptr<ProgramNode>& getAST();
private:
	int pos;
	std::vector<Token> TokenList;
	std::unique_ptr<ProgramNode> programAST;
	bool isVarParse;

	std::unique_ptr<ASTNode> parseVarAST(int currPos);
	std::unique_ptr<ASTNode> parseExpressionAST();
	std::unique_ptr<ASTNode> parseTermAST();
	std::unique_ptr<ASTNode> parseFactorAST();
	std::unique_ptr<ASTNode> parseAssignementAST(int currPos);
	std::unique_ptr<ASTNode> parsePrintAST();

	bool parseVar();
	bool match(TokenType type);
	bool match(TokenType type, std::string valWaiting);
	void err(std::string msg);
	std::string tokenTypeToStr(TokenType token);
	bool parseAssignement();
	bool parseStatement();
	bool parsePrint();
};

#endif