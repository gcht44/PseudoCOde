#include "Lexer.hpp"

// TODO: Changer le systéme de lecture de ligne par ligne en tout d'un coup

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
    this->nbLigne = 1;
	std::ifstream fichier(f); // Ouvre le fichier en lecture
    if (!fichier) {
        std::cerr << "Erreur : impossible d'ouvrir le fichier." << std::endl;
    }

    std::stringstream buffer;
    buffer << fichier.rdbuf();

    // Récupérer le contenu du stringstream dans une std::string
    this->contenu = buffer.str();

    // Remplacer les sauts de ligne par "\n"
    for (size_t pos = 0; (pos = contenu.find('\n', pos)) != std::string::npos; ++pos) {
        contenu.replace(pos, 1, "\\n");
    }


    fichier.close(); 
}

Token Lexer::readIdentifierOrKeyword()
{
    std::string value;
    while (std::isalnum(contenu[pos])) { value.push_back(contenu[pos++]); } // la boucle va s'arreter au premier espace qu'elle voit
    if (value == "ENTIER") return Token(TokenType::KEYWORD, value, nbLigne + 1, pos);
    if (value == "BOOLEAN") return Token(TokenType::KEYWORD, value, nbLigne + 1, pos);
    if (value == "REEL") return Token(TokenType::KEYWORD, value, nbLigne + 1, pos);
    if (value == "STRING") return Token(TokenType::KEYWORD, value, nbLigne + 1, pos);
    if (value == "TRUE") return Token(TokenType::KEYWORD, value, nbLigne + 1, pos);
    if (value == "FALSE") return Token(TokenType::KEYWORD, value, nbLigne + 1, pos);
    if (value == "print") return Token(TokenType::KEYWORD, value, nbLigne + 1, pos);
    if (value == "VARIABLE") return Token(TokenType::KEYWORD, value, nbLigne + 1, pos);
    if (value == "DEBUT") return Token(TokenType::KEYWORD, value, nbLigne + 1, pos);
    if (value == "ET") return Token(TokenType::KEYWORD, value, nbLigne + 1, pos);
    if (value == "OU") return Token(TokenType::KEYWORD, value, nbLigne + 1, pos);
    if (value == "SI") return Token(TokenType::KEYWORD, value, nbLigne + 1, pos);
    if (value == "SINON") return Token(TokenType::KEYWORD, value, nbLigne + 1, pos);
    return Token(TokenType::IDENTIFIER, value, nbLigne + 1, pos);
}

Token Lexer::readNumber()
{
    std::string value;
    while (std::isdigit(contenu[pos])) { value.push_back(contenu[pos++]); }
    return Token(TokenType::NUMBER, value, nbLigne + 1, pos);
}

Token Lexer::GetNextToken()
{
    // Token token;
    std::vector<Token> TokenList;
    int nbSpace = 0;
    if (pos >= this->contenu.size()) { pos++; return Token(TokenType::END, "", nbLigne, pos); }

    while (std::isspace(contenu[pos])) { pos++; nbSpace++; }
    char c = contenu[pos];

    if (std::isalpha(c)) return readIdentifierOrKeyword(); // dès qu'on voit une lettre on part du principe que cest soit un id ou un keyword on fera la diff dans readIdentifierOrKeyword()
    if (std::isdigit(c)) return readNumber(); // dès qu'on voit un chiffre on part du principe que cest un nombre

    switch (c)
    {
        case ';': pos++; return Token(TokenType::SEMICOLON, ";", nbLigne + 1, pos);
        case '(': pos++; return Token(TokenType::LPAREN, "(", nbLigne + 1, pos);
        case ')': pos++; return Token(TokenType::RPAREN, ")", nbLigne + 1, pos);
        case '+': pos++; return Token(TokenType::PLUS, "+", nbLigne + 1, pos);
        case '-': pos++; return Token(TokenType::SUB, "-", nbLigne + 1, pos);
        case '*': pos++; return Token(TokenType::MULT, "*", nbLigne + 1, pos);
        case '/': pos++; return Token(TokenType::DIV, "/", nbLigne + 1, pos);
        case ':': pos++; return Token(TokenType::COLON, ":", nbLigne + 1, pos);
        case '.': pos++; return Token(TokenType::DOT, ".", nbLigne + 1, pos);
        case ',': pos++; return Token(TokenType::COMMA, ",", nbLigne + 1, pos);
        case '"': pos++; return Token(TokenType::QUOTE, "\"", nbLigne + 1, pos);
        case '{': pos++; return Token(TokenType::LEFT_BRACE, "{", nbLigne + 1, pos);
        case '}': pos++; return Token(TokenType::RIGHT_BRACE, "}", nbLigne + 1, pos);

        case '=': 
            pos++; 
            if (contenu[pos] == '=') { pos++; return Token(TokenType::EQUAL_EQUAL, "==", nbLigne, pos); }
            return Token(TokenType::EQUALS, "=", nbLigne + 1, pos);

        case '>': 
            pos++;
            if (contenu[pos] == '=') { pos++; return Token(TokenType::GREATHER_EQUAL, ">=", nbLigne, pos); }
            return Token(TokenType::GREATHER, ">", nbLigne + 1, pos);

        case '<': 
            pos++; 
            if (contenu[pos] == '=') { pos++; return Token(TokenType::LESS_EQUAL, "<=", nbLigne, pos); }
            return Token(TokenType::LESS, "<", nbLigne + 1, pos);
        case '!':
            pos++;
            if (contenu[pos] == '=') { pos++; return Token(TokenType::NOT_EQUAL, "!=", nbLigne, pos); }
            break;
        case '\\':
            pos++;
            if (contenu[pos] == 'n') { pos++; nbLigne++; return GetNextToken(); }
            break;
    }

    std::cerr << "[LEXER] ERR: Charactere " << contenu[pos] << "(ASCII: " << (int)contenu[pos] << ") non defini" << std::endl;
    std::cerr << "Ligne: " << nbLigne << " Colonne: " << pos << std::endl;
    std::cerr << contenu[pos] << std::endl;
    exit(1);
}

std::vector<Token> Lexer::Tokenise()
{
    std::vector<Token> TokensList; 
    int i = 0;
    /*while (true)
    {
        // if (isVide(contenu[nbLigne])) this->nbLigne++;
        
        Token token = GetNextToken(contenu);
        TokensList.push_back(token);
        if (token.type == TokenType::END) break;
        if (this->pos >= this->contenu[nbLigne].size())
        {
            this->pos = 0;
            this->nbLigne++;
        }
    }*/
    // std::cout << contenu.size() << std::endl;
    while (pos <= contenu.size())
    {
        TokensList.push_back(GetNextToken());
        // std::cout << pos << std::endl;
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
        case TokenType::COMMA: s = "Type: COMMA, "; break;
        case TokenType::QUOTE: s = "Type: QUOTE, "; break;
        case TokenType::LESS: s = "Type: LESS, "; break;
        case TokenType::LESS_EQUAL: s = "Type: LESS_EQUAL, "; break;
        case TokenType::GREATHER: s = "Type: GREATHER, "; break;
        case TokenType::GREATHER_EQUAL: s = "Type: GREATHER_EQUAL, "; break;
        case TokenType::NOT_EQUAL: s = "Type: NOT_EQUAL, "; break;
        case TokenType::RIGHT_BRACE: s = "Type: RIGHT_BRACE, "; break;
        case TokenType::LEFT_BRACE: s = "Type: LEFT_BRACE, "; break;
        case TokenType::END: s = "Type: END, "; break;

        }

        std::clog << "[" << i << "] " << s << "Value: " << t[i].value << std::endl;
    }
}