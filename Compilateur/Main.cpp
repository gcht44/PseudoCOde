#include <iostream>
#include "Lexer.hpp"
#include "Parser.hpp"
#include "SemantiqueAnalize.hpp"
#include "ByteCode.hpp"

int main()
{
	Lexer lexer("test.txt");

	std::vector<Token> TokenList = lexer.Tokenise();

	std::clog << "[LEXER] Tokenisation OK" << std::endl;
	lexer.printTokens(TokenList);


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


	/*AnalizeSemantique as;
	as.allAnalize(parser.getAST()->getTableAST());


	std::cout << "\n\n\n";


	ByteCode bc;
	bc.generateAllByteCode(parser.getAST()->getTableAST());
	bc.printByteCode();


	std::cout << "\n\n\n";

	std::cout << "Execute:\n";
	bc.executeByteCode();*/


	return 0;
}