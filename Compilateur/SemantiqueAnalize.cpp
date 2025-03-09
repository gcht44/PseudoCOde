#include "SemantiqueAnalize.hpp"

void AnalizeSemantique::err(std::string msg)
{
	std::cerr << "[Analyse Semantique] ERR: " << msg << std::endl;
	exit(1);
}

bool AnalizeSemantique::analize(ASTNode* node, SymbolTable& symbolTable)
{

		/*
		dynamic_cast: convertit un pointeur ou reference vers un type dérivé seulement si la convertion est valide sinon ca sera un null_ptr
		dans notre cas si ASTTable[i].get() est un VarDeclarationNode la condition s'effectuera on peut ensuite utiliser la variable varDecl
		ASTTable[i].get(): permet de récupérer le pointeur brut
		*/
	if (auto varDecl = dynamic_cast<const VarDeclarationNode*>(node))
	{
		// std::cout << "VarDecl" << std::endl;
		if (symbolTable.getVariableType(varDecl->getName()) == Type::NONE)
		{
			err("Redéclaration de variable.");
		}
		// symbolTable[varDecl->getName()] = true;
		analize(varDecl->getExpr().get(), symbolTable);
		varDecl->checkType(symbolTable);
	}
	else if (auto assignment = dynamic_cast<const AssignmentNode*>(node))
	{
		// std::cout << "Assignment" << std::endl;
		if (symbolTable.getVariableType(assignment->getName()) != Type::NONE)
		{
			analize(assignment->getExpr().get(), symbolTable);
			assignment->checkType(symbolTable);
			return true;
		}
		else
		{
			err("Variable " + assignment->getName() + " non déclaré.");
		}
	}

	else if (auto identifier = dynamic_cast<const IdentifierNode*>(node))
	{
		// std::cout << "Identifier" << std::endl;
		if (symbolTable.getVariableType(identifier->getName()) != Type::NONE)
		{
			return true;
		}
		else
		{
			err("Identifiant " + identifier->getName() + " non déclaré.");
		}
	}
	else if (auto binaryOpNode = dynamic_cast<const BinaryOpNode*>(node))
	{
		// std::cout << "Operation" << std::endl;
		analize(binaryOpNode->getLeft().get(), symbolTable);
		analize(binaryOpNode->getRight().get(), symbolTable);
	}
	else if (auto printNode = dynamic_cast<const PrintNode*>(node))
	{
		// std::cout << "Print" << std::endl;
		analize(printNode->getExpr().get(), symbolTable);
	}
	else if (auto entierNode = dynamic_cast<const IntNode*>(node))
	{
		return true;
	}
	else if (auto reelNode = dynamic_cast<const ReelNode*>(node))
	{
		return true;
	}
	else if (auto boolNode = dynamic_cast<const BoolNode*>(node))
	{
		return true;
	}
	else
	{
		std::cerr << "[SEMANTIQUE ANALYSE] ERR Noeud Inconnu" << std::endl;
		return false;
	}
}

void AnalizeSemantique::allAnalize(const std::vector<std::unique_ptr<ASTNode>>& AST, SymbolTable& symbolTable)
{
	for (int i = 0; i < AST.size(); i++)
	{
		analize(AST[i].get(), symbolTable);
	}
	std::clog << "[SEMANTIQUE ANALYSE] Analyse Sémantique OK" << std::endl;
}

AnalizeSemantique::AnalizeSemantique()
{

}