#include <iostream>
#include <fstream>
#include <sstream>

#include "Lexer.hpp"
#include "Parser.hpp"
#include "SemantiqueAnalize.hpp"
#include "ByteCode.hpp"

int main()
{
	// Lire le contenu du fichier
	const std::string filename = "test1.txt";
	std::ifstream file(filename);
	if (!file) {
		std::cerr << "Impossible d'ouvrir le fichier: " << filename << std::endl;
		return 1;
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string contenu = buffer.str();
	file.close();

	Lexer lexer(contenu);

	std::vector<Token> TokenList = lexer.Tokenise();
	std::cout << TokenList.size();

	lexer.printTokens(TokenList);
	std::clog << "[LEXER] Tokenisation OK" << std::endl;


	std::cout << "\n\n\n";
	

	Parser parser(TokenList);
	if (parser.parseProg())
	{
		std::clog << "[PARSER] Analyse Synthaxique OK" << std::endl;
	}
	else
	{
		std::cerr << "[PARSER] ERR: Échec de l'analyse synthaxique" << std::endl;
	}
	parser.getAST()->print();


	std::cout << "\n\n\n";


	AnalizeSemantique as;
	as.allAnalize(parser.getAST()->getTableAST(), parser.getSymbolTable());


	std::cout << "\n\n\n";


	try {
		ByteCode bc;
		bc.generateAllByteCode(parser.getAST()->getTableAST(), parser.getSymbolTable());
		bc.printByteCode();

		std::cout << "\n\n\n";

		std::cout << "Execute:\n";
		bc.executeByteCode();
	}
	catch (const std::runtime_error& e) {
		std::cerr << "Erreur runtime: " << e.what() << std::endl;
	}
	catch (const std::exception& e) {
		std::cerr << "Erreur standard: " << e.what() << std::endl;
	}
	
	

	return 0;
}