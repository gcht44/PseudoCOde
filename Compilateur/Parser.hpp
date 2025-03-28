#ifndef PARSER_HPP
#define PARSER_HPP

#include <iostream>
#include <vector>
#include "Lexer.hpp"
#include "AST.hpp"
#include "SymbolTable.hpp"

class Parser {

public:
	Parser(std::vector<Token> tokens);
	bool parseProg();
	std::unique_ptr<ProgramNode>& getAST();
	SymbolTable& getSymbolTable();
private:
	int pos;
	std::vector<Token> TokenList;
	std::unique_ptr<ProgramNode> programAST;
	SymbolTable symbolTable;
	bool isVarParse;
	int currentIndent;
	// SymbolTable symbolTable;

	int getTokenPrecedence();
	std::unique_ptr<ASTNode> parseBinOpRHS(int ExprPrec, std::unique_ptr<ASTNode> LHS);
	std::unique_ptr<ASTNode> parseExpressionAST();
	std::unique_ptr<ASTNode> parseTermAST();
	std::unique_ptr<ASTNode> parseFactorAST();
	std::unique_ptr<BlockNode> parseBlock();

	std::unique_ptr<ASTNode> parseIf();
	bool match(TokenType type);
	bool match(TokenType type, std::string valWaiting);
	void err(std::string msg);
	std::string tokenTypeToStr(TokenType token);
	std::unique_ptr<ASTNode> parseAssignement();
	std::unique_ptr<ASTNode> parseVar();
	std::unique_ptr<ASTNode> parseTantQue();
	std::unique_ptr<ASTNode> parsePour();
	std::unique_ptr<ASTNode> parseStatement();
	std::unique_ptr<ASTNode> parsePrint();
};

#endif