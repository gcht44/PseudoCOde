#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>

#include "Lexer.hpp"
#include "Parser.hpp"
#include "SemantiqueAnalize.hpp"
#include "ByteCode.hpp"
#include "Debugger.hpp"



bool verifierExtension(std::string nomFichier, std::string extension) {
	size_t pos = nomFichier.find_last_of(".");
	if (pos == std::string::npos) {
		std::cout << " Aucune extension trouvée pour : " << nomFichier << std::endl;
		return false;
	}

	std::string extFichier = nomFichier.substr(pos + 1);

	// Debug pour voir ce qui est extrait
	// std::cout << " Fichier : " << nomFichier << " | Extension trouvée : " << extFichier << std::endl;

	return extFichier == extension;
}

int main(int argc, char* argv[])
{
	// std::string arg = "debug"; // temporaire

	std::string filename;
	bool FILESELECT = false;
	bool DEBUG = false;
	bool DEBUGFORDEV = false;

	// std::cout << "Nombre d'arguments : " << argc << std::endl;

	for (int i = 1; i < argc; i++)
	{
		std::string arg = argv[i];
		if (arg == "-o")
		{
			filename = argv[++i];
			if (!verifierExtension(filename, "pc")) { std::cerr << "Extension incorrect: Attendu -> .pc."; exit(1); }
			
			FILESELECT = true;
		}
		if (arg == "-d" || arg == "--debug")
		{
			DEBUG = true;
		}
		if (arg == "-dd" || arg == "--debugdev")
		{
			DEBUGFORDEV = true;
		}

	}
	if (!FILESELECT)
	{
		std::cerr << "Aucun fichier spécifier: Attendu: pcc -o <nom_fichier>.<ext>."; 
		exit(1);
	}
	
	
	
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

	// lexer.printTokens(TokenList);
	std::clog << "[LEXER] Tokenisation OK" << std::endl;


	// std::cout << "\n\n\n";
	

	Parser parser(TokenList);
	if (parser.parseProg())
	{
		std::clog << "[PARSER] Analyse Synthaxique OK" << std::endl;
	}
	else
	{
		std::cerr << "[PARSER] ERR: Échec de l'analyse synthaxique" << std::endl;
	}
	 // parser.getAST()->print();


	// std::cout << "\n\n\n";


	AnalizeSemantique as;
	as.allAnalize(parser.getAST()->getTableAST(), parser.getSymbolTable());


	// std::cout << "\n\n\n";


	try {
		ByteCode bc;
		bc.generateAllByteCode(parser.getAST()->getTableAST(), parser.getSymbolTable());
		// bc.printByteCode();

		if (DEBUGFORDEV)
		{
			std::cout << "\n\n\n";

			std::string argPcdd = "";
			int indexByteCode = 0;
			std::vector<Instruction>& BytecodeList = bc.getBC();

			std::cout << "Vous êtes dans le mode Debogueur PseudoCode pouur les développeur du compilateur. Il n'est donc pas réserver à tous le monde et n'est pas fait pour être compris. \n (pcdd) help -> si vous avez toute fois besoin d'aide\n" << std::endl;
			
			while (argPcdd != "exit")
			{

				std::cout << "(pcdd) ";
				std::cin >> argPcdd;

				if (argPcdd == "help")
				{
					std::cout << "- Commande du PCDD -" << std::endl;
					std::cout << "printToken            Affiche les tokens" << std::endl;
					std::cout << "printAST              Affiche l'AST" << std::endl;
					std::cout << "printBytecode         Affiche le bytecode" << std::endl;
					std::cout << "current               Affiche l'opcode actuel" << std::endl;
					std::cout << "stacks                Affiche les piles" << std::endl;
					std::cout << "next                  Execute l'opcode suivant" << std::endl;
				}
				else if (argPcdd == "exit") { continue; }
				else if (argPcdd == "printToken") { lexer.printTokens(TokenList); }
				else if (argPcdd == "printAST") { parser.getAST()->print(); std::cout << "\n"; }
				else if (argPcdd == "printBytecode") { bc.printByteCode(); }
				else if (argPcdd == "current") 
				{ 
					if (indexByteCode - 1 == -1) { std::cout << "Veuillez executer next pour executer le premier opcode." << std::endl; continue; }
					std::cout << "[" << indexByteCode-1 << "] ";
					bc.printOneByteCode(BytecodeList[indexByteCode-1]);
				}
				else if (argPcdd == "next")
				{
					bc.executeByteCode(BytecodeList[indexByteCode++], indexByteCode);
				}
				else if (argPcdd == "stacks") 
				{ 
					bc.printStacks();
				}
				else
				{
					std::cout << "Commande inconnu" << std::endl;
				}
			}
		}
		else
		{
			std::cout << "Execute:\n";
			bc.executeByteCode();
		}
	}
	catch (const std::runtime_error& e) {
			std::cerr << "Erreur runtime: " << e.what() << std::endl;
	}
	catch (const std::exception& e) {
		std::cerr << "Erreur standard: " << e.what() << std::endl;
	}
	

	return 0;
}