#ifndef SEMANTIQUE_HPP
#define SEMANTIQUE_HPP

#include <map>
#include <iostream>
#include <vector>
#include "AST.hpp"

class AnalizeSemantique {
private:
	std::map<std::string, bool> symbolTable;
	// const std::vector<std::unique_ptr<ASTNode>>& ASTTable;
	void err(std::string msg);
	bool analize(ASTNode* node, SymbolTable& symbolTable);
	
public:
	AnalizeSemantique();
	void allAnalize(const std::vector<std::unique_ptr<ASTNode>>& AST, SymbolTable& symbolTable);

};

#endif