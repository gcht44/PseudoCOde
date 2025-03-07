#include "Lexer.hpp"

bool Lexer::isVide(const std::string& ligne) {
    for (char ch : ligne) {
        if (!std::isspace(ch)) {
            return false; // La ligne contient un caractère non-espace
        }
    }
    return true; // La ligne est vide ou ne contient que des espaces
}

Lexer::Lexer(std::string f) 
{
    this->pos = 0;
    this->nbLigne = 0;
	std::ifstream fichier(f); // Ouvre le fichier en lecture
    if (!fichier) {
        std::cerr << "Erreur : impossible d'ouvrir le fichier." << std::endl;
    }

    std::string ligne;
    // Lire le fichier ligne par ligne
    while (std::getline(fichier, ligne)) {
        // Traiter chaque ligne
        this->contenu.push_back(ligne);
    }

    fichier.close(); 
}

Token Lexer::readIdentifierOrKeyword(std::string ligne)
{
    std::string value;
    while (std::isalnum(ligne[pos])) { value.push_back(ligne[pos++]); } // la boucle va s'arreter au premier espace qu'elle voit
    if (value == "ENTIER") return Token(TokenType::KEYWORD, value, nbLigne + 1, pos);
    if (value == "BOOLEAN") return Token(TokenType::KEYWORD, value, nbLigne + 1, pos);
    if (value == "REEL") return Token(TokenType::KEYWORD, value, nbLigne + 1, pos);
    if (value == "TRUE") return Token(TokenType::KEYWORD, value, nbLigne + 1, pos);
    if (value == "FALSE") return Token(TokenType::KEYWORD, value, nbLigne + 1, pos);
    if (value == "print") return Token(TokenType::KEYWORD, value, nbLigne + 1, pos);
    if (value == "VARIABLE") return Token(TokenType::KEYWORD, value, nbLigne + 1, pos);
    if (value == "DEBUT") return Token(TokenType::KEYWORD, value, nbLigne + 1, pos);
    return Token(TokenType::IDENTIFIER, value, nbLigne + 1, pos);
}

Token Lexer::readNumber(std::string ligne)
{
    std::string value;
    while (std::isdigit(ligne[pos])) { value.push_back(ligne[pos++]); }
    return Token(TokenType::NUMBER, value, nbLigne + 1, pos);
}

Token Lexer::GetNextToken(std::string ligne)
{
    // Token token;
    std::vector<Token> TokenList;

    // if (nbLigne >= this->contenu.size()) return Token(TokenType::END, "");
    
    while (std::isspace(ligne[pos])) { pos++; }


    char c = ligne[pos];

    if (std::isalpha(c)) return readIdentifierOrKeyword(ligne); // dès qu'on voit une lettre on part du principe que cest soit un id ou un keyword on fera la diff dans readIdentifierOrKeyword()
    if (std::isdigit(c)) return readNumber(ligne); // dès qu'on voit un chiffre on part du principe que cest un nombre

    switch (c)
    {
        case '=': pos++;  return Token(TokenType::EQUALS, "=", nbLigne + 1, pos);
        case ';': pos++; return Token(TokenType::SEMICOLON, ";", nbLigne + 1, pos);
        case '(': pos++; return Token(TokenType::LPAREN, "(", nbLigne + 1, pos);
        case ')': pos++; return Token(TokenType::RPAREN, ")", nbLigne + 1, pos);
        case '+': pos++; return Token(TokenType::PLUS, "+", nbLigne + 1, pos);
        case '-': pos++; return Token(TokenType::SUB, "-", nbLigne + 1, pos);
        case '*': pos++; return Token(TokenType::MULT, "*", nbLigne + 1, pos);
        case '/': pos++; return Token(TokenType::DIV, "/", nbLigne + 1, pos);
        case ':': pos++; return Token(TokenType::COLON, ":", nbLigne + 1, pos);
        case '.': pos++; return Token(TokenType::DOT, ".", nbLigne + 1, pos);
    }

    std::cerr << "[LEXER] ERR: Charactere " << ligne[pos] << " non defini" << std::endl;
    std::cerr << "Ligne: " << nbLigne << " Colonne: " << pos << std::endl;
    std::cerr << ligne << std::endl;
    exit(1);
}


std::vector<Token> Lexer::Tokenise()
{
    std::vector<Token> TokensList; 
    int i = 0;
    while (true)
    {
        

        if (nbLigne >= this->contenu.size())
        {
            TokensList.push_back(Token(TokenType::END, "", nbLigne+1, pos));
            break;
        }
        if (isVide(contenu[nbLigne])) this->nbLigne++;
        
        Token token = GetNextToken(contenu[nbLigne]);
        TokensList.push_back(token);
        // if (token.type == TokenType::END) break;
        if (this->pos >= this->contenu[nbLigne].size())
        {
            this->pos = 0;
            this->nbLigne++;
        }
    }

    return TokensList;
}


void Lexer::printTokens(std::vector<Token> t)
{
    for (int i = 0; i < t.size(); i++)
    {
        std::string s;
        switch (t[i].type)
        {
        case TokenType::EQUALS: s = "Type: EQUALS, "; break;
        case TokenType::IDENTIFIER: s = "Type: IDENTIFIER, "; break;
        case TokenType::KEYWORD: s = "Type: KEYWORD, "; break;
        case TokenType::LPAREN: s = "Type: LPAREN, "; break;
        case TokenType::NUMBER: s = "Type: NUMBER, "; break;
        case TokenType::PLUS: s = "Type: PLUS, "; break;
        case TokenType::RPAREN: s = "Type: RPAREN, "; break;
        case TokenType::SEMICOLON: s = "Type: SEMICOLON, "; break;
        case TokenType::SUB: s = "Type: SUB, "; break;
        case TokenType::MULT: s = "Type: MULT, "; break;
        case TokenType::DIV: s = "Type: DIV, "; break;
        case TokenType::COLON: s = "Type: COLON, "; break;
        case TokenType::DOT: s = "Type: DOT, "; break;
        case TokenType::END: s = "Type: END, "; break;

        }

        std::clog << i << s << "Value: " << t[i].value << std::endl;
    }
}