#include "Parser.hpp"

Parser::Parser(std::vector<Token> tokens)
{ 
	this->pos = 0;
	this->programAST = std::make_unique<ProgramNode>();
	this->isVarParse = true;
	for (int i=0; i < tokens.size(); i++)
	{
		this->TokenList.push_back(tokens[i]);
	}
}


// Verifie que le type du token est equivalent a celui passer en argument
// Utilisation: Permet de verifier qu'un Token a un type conforme à la synthaxe
bool Parser::match(TokenType type, std::string valWaiting)
{
	if ((this->TokenList[pos].type == type) && (this->TokenList[pos].value == valWaiting))
	{
		pos++;
		return true;
	}
	return false;
}

bool Parser::match(TokenType type)
{
	if (this->TokenList[pos].type == type)
	{
		pos++;
		return true;
	}
	return false;
}

std::string Parser::tokenTypeToStr(TokenType type)
{
	switch (type)
	{
	case TokenType::EQUALS: return "Type: EQUALS, "; break;
	case TokenType::IDENTIFIER: return "Type: IDENTIFIER, "; break;
	case TokenType::KEYWORD: return "Type: KEYWORD, "; break;
	case TokenType::LPAREN: return "Type: LPAREN, "; break;
	case TokenType::NUMBER: return "Type: NUMBER, "; break;
	case TokenType::PLUS: return "Type: PLUS, "; break;
	case TokenType::RPAREN: return "Type: RPAREN, "; break;
	case TokenType::SEMICOLON: return "Type: SEMICOLON, "; break;
	case TokenType::END: return "Type: END, "; break;
	}
}

void Parser::err(std::string msg)
{
	std::cerr << "Ligne: " << TokenList[pos].li << " Colonne: " << TokenList[pos].col << std::endl;
	std::cerr << "	[PARSER] ERR: " << msg << " au token " << tokenTypeToStr(TokenList[pos].type) << std::endl;
	exit(1);
}


/*
var x = 2;
x = 5;
*/
bool Parser::parseAssignement()
{

	if (match(TokenType::IDENTIFIER))
	{
		int currPos = pos-1;
		if (!match(TokenType::EQUALS)) { err("'=' attendu apres l'identifiant"); }
		this->programAST->addStatement(parseAssignementAST(currPos));
		if (!match(TokenType::SEMICOLON)) { err("';' attendu a la fin de la declaration"); }
		// std::cout << TokenList[currPos].value << std::endl;
		return true;
	}
	return false;
}

std::unique_ptr<ASTNode> Parser::parseTermAST()
{
	std::unique_ptr<ASTNode> left = parseFactorAST();
	while (match(TokenType::DIV) || match(TokenType::MULT)) {
		
		std::string op = this->TokenList[pos-1].value; // Récupère l'opérateur 
		std::unique_ptr<ASTNode> right = parseTermAST(); // Parse le terme suivant (b, puis c, puis d)
		left = std::make_unique<BinaryOpNode>(op, std::move(left), std::move(right)); // Crée un nœud d'addition
	}
	return left;

}

std::unique_ptr<ASTNode> Parser::parseFactorAST()
{
	if (match(TokenType::LPAREN)) { // Gère les parenthèses
		std::unique_ptr<ASTNode> expr = parseExpressionAST(); // Parse l'expression à l'intérieur des parenthèses
		if (!match(TokenType::RPAREN)) throw "Erreur: parenthèse fermante manquante";
		return expr;
	}
	else if (match(TokenType::NUMBER)) { // Gère les nombres
		std::unique_ptr<ASTNode> num = std::make_unique<IntNode>(this->TokenList[pos - 1].value);
		// pos++;
		return num;
	}
	else if (match(TokenType::IDENTIFIER)) { // Gère les variables
		std::unique_ptr<ASTNode> var = std::make_unique<IdentifierNode>(this->TokenList[pos - 1].value);
		// pos++;
		return var;
	}
	else if (match(TokenType::KEYWORD, "TRUE") || match(TokenType::KEYWORD, "FALSE")) { 
		std::unique_ptr<ASTNode> bool_ = std::make_unique<BoolNode>(this->TokenList[pos - 1].value);
		// pos++;
		return bool_;
	}
	else {
		err("Token Inconnu");
	}
}

std::unique_ptr<ASTNode> Parser::parseExpressionAST() {

	std::unique_ptr<ASTNode> left = parseTermAST();
	while (match(TokenType::PLUS) || match(TokenType::SUB)) {
		std::string op = this->TokenList[pos-1].value; // Récupère l'opérateur '+'
		std::unique_ptr<ASTNode> right = parseTermAST(); // Parse le terme suivant (b, puis c, puis d)
		left = std::make_unique<BinaryOpNode>(op, std::move(left), std::move(right)); // Crée un nœud d'addition
	}
	
	return left;
}

std::unique_ptr<ASTNode> Parser::parseVarAST(int currPos)
{
	std::string name = this->TokenList[currPos].value;
	// currPos += 2;
	return std::make_unique<VarDeclarationNode>(name, parseExpressionAST());
}

std::unique_ptr<ASTNode> Parser::parseAssignementAST(int currPos)
{
	std::string name = this->TokenList[currPos].value;
	currPos += 2;
	return std::make_unique<AssignmentNode>(name, parseExpressionAST());
}

std::unique_ptr<ASTNode> Parser::parsePrintAST()
{
	return std::make_unique<PrintNode>(parseExpressionAST());
}

bool Parser::parseVar()
{

	if (match(TokenType::KEYWORD, "ENTIER"))
	{
		int currPos = pos;

		if (!match(TokenType::IDENTIFIER)) { err("Identifiant attendu apres 'var'"); } // Par exemple ici on verifie que le token après la decaration de la var est bien un id
		if (!match(TokenType::EQUALS)) { err("'=' attendu apres l'identifiant"); }
		this->programAST->addStatement(parseVarAST(currPos));
		if (!match(TokenType::SEMICOLON)) { err("';' attendu a la fin de la declaration"); }	
		return true;
	}
	else if (match(TokenType::KEYWORD, "BOOLEAN"))
	{
		int currPos = pos;

		if (!match(TokenType::IDENTIFIER)) { err("Identifiant attendu apres 'var'"); } // Par exemple ici on verifie que le token après la decaration de la var est bien un id
		if (!match(TokenType::EQUALS)) { err("'=' attendu apres l'identifiant"); }
		this->programAST->addStatement(parseVarAST(currPos));
		if (!match(TokenType::SEMICOLON)) { err("';' attendu a la fin de la declaration"); }
		return true;
	}
		
	return false;
}

bool Parser::parsePrint()
{
	if (match(TokenType::KEYWORD, "print"))
	{
 		int currPos = pos+1;
		if (!match(TokenType::LPAREN)) { err("'(' attendu apres 'print'"); }
		this->programAST->addStatement(parsePrintAST());
		if (!match(TokenType::RPAREN)) { err("')' attendu apres 'print'"); }
		if (!match(TokenType::SEMICOLON)) { err("';' attendu a la fin de la declaration"); }
		return true;
	}
	return false;
}

bool Parser::parseStatement() {
	return parseVar() ||  parsePrint() || parseAssignement();
}

bool Parser::parseProg()
{
	while (this->TokenList[pos].type != TokenType::END)
	{
		if (match(TokenType::KEYWORD, "VARIABLE") && match(TokenType::COLON))
		{
			while (true)
			{

				parseVar();
				if (match(TokenType::KEYWORD, "DEBUT")) { break; }
				// else { err("On peut uniquement déclarer des variables ici"); }
			}
			if (!match(TokenType::COLON)) { err("':' attendu a la fin de DEBUT"); }
			this->isVarParse = false;
		}
		else if (!this->isVarParse)
		{
			if (!parseStatement()) { err("Invalid statement"); };
		}
		else
		{
			err("Synthaxe de base non respécter");
		}
	}

	return true;
}

std::unique_ptr<ProgramNode>& Parser::getAST()
{
	return programAST;
}