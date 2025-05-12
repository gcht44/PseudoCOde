#include "Lexer.hpp"

bool Lexer::IsEOF() const 
{
    return pos >= contenu.length();
}

void Lexer::SkipWhitespace() {
    while (!IsEOF() && std::isspace(CurrentChar())) 
    {
        if (CurrentChar() == '\n') {
            nbLigne++;
        }
        NextChar();
    }
}

char Lexer::CurrentChar() const 
{
    if (pos < contenu.length()) 
    {
        return contenu[pos];
    }
    return '\0'; // Caractère nul pour indiquer la fin du fichier
}

char Lexer::NextChar()  
{
    if (pos < contenu.length()) 
    {
        return contenu[++pos];
    }
    return '\0'; // Caractère nul pour indiquer la fin du fichier
}

bool Lexer::isVide(const std::string& line) const 
{
    for (char c : line) 
    {
        if (!std::isspace(c)) 
        {
            return false;
        }
    }
    return true;
}

int Lexer::calculateIndentation(const std::string& line) const 
{
    int spaces = 0;
    for (char c : line) {
        if (c == ' ') {
            spaces++;
        }
        else if (c == '\t') {
            spaces += INDENT_SIZE; // Une indent vaut IDENT_SIZE
        }
        else {
            break;
        }
    }

    // Vérifier que l'indentation est un multiple de INDENT_SIZE
    if (spaces % INDENT_SIZE != 0) {
        std::cerr << "Erreur: Indentation incorrecte à la ligne " << nbLigne << std::endl;
        // Vous pourriez lancer une exception ici
    }

    return spaces / INDENT_SIZE; // Retourner le niveau d'indentation
}

std::vector<Token> Lexer::handleIndentation(const std::string& line) 
{
    std::vector<Token> indentTokens;

    if (isVide(line)) {
        return indentTokens; // Ignorer les lignes vides
    }

    int lineIndent = calculateIndentation(line);

    if (lineIndent > currentIndent) 
    {
        // Vérifier que l'indentation augmente d'un seul niveau à la fois
        if (lineIndent - currentIndent > 1) 
        {
            std::cerr << "Erreur: Augmentation d'indentation trop importante à la ligne " << nbLigne << std::endl;
            // Vous pourriez lancer une exception ici
        }

        // Augmentation de l'indentation
        indentStack.push(currentIndent);
        currentIndent = lineIndent;
        indentTokens.push_back(Token(TokenType::INDENT, "", nbLigne, pos));
    }
    else if (lineIndent < currentIndent) 
    {
        // Diminution de l'indentation
        while (!indentStack.empty() && lineIndent < currentIndent) 
        {
            currentIndent = indentStack.top();
            indentStack.pop();
            indentTokens.push_back(Token(TokenType::DEDENT, "", nbLigne, 0));
        }

        // Vérifier si l'indentation correspond à un niveau précédent
        if (lineIndent != currentIndent) 
        {
            std::cerr << "Erreur: Indentation incohérente à la ligne " << nbLigne << std::endl;
            // Vous pourriez lancer une exception ici
        }
    }

    return indentTokens;
}

std::vector<Token> Lexer::Tokenise() {
    std::vector<Token> tokens;
    indentStack = std::stack<int>();
    indentStack.push(0);
    currentIndent = 0;
    pos = 0; // Position globale dans tout le contenu
    nbLigne = 1;

    std::istringstream stream(contenu);
    std::string line;
    size_t contentPos = 0; // Pour suivre la position dans le contenu complet

    while (std::getline(stream, line)) {
        // Gérer les indentations
        std::vector<Token> indentTokens = handleIndentation(line);
        tokens.insert(tokens.end(), indentTokens.begin(), indentTokens.end());

        // Traiter les tokens de la ligne non vide
        if (!isVide(line)) {
            int indentSize = calculateIndentation(line) * INDENT_SIZE;
            pos = contentPos; // Position absolue dans contenu

            size_t linePos = indentSize; // Position relative dans la ligne
            while (linePos < line.length()) {
                Token token = GetNextToken();
                if (token.type != TokenType::END) {
                    tokens.push_back(token);
                    linePos = pos - contentPos; // Mettre à jour la position relative
                }
                else {
                    break;
                }
            }

            // Ajouter un token NEWLINE à la fin de chaque ligne non vide
            tokens.push_back(Token(TokenType::NEWLINE, "\\n", nbLigne, line.length()));
        }

        contentPos += line.length() + 1; // +1 pour le caractère de nouvelle ligne
        nbLigne++;
    }

    // Ajouter les DEDENT manquants à la fin du fichier
    while (currentIndent > 0) {
        currentIndent = indentStack.top();
        indentStack.pop();
        tokens.push_back(Token(TokenType::DEDENT, "", nbLigne, 0));
    }

    tokens.push_back(Token(TokenType::END, "", nbLigne, 0));
    return tokens;
}

// Implémentation des autres méthodes du lexer...
Token Lexer::GetNextToken()
{
    // Cette méthode reconnaît les différents tokens
    // comme les identifiants, nombres, chaînes, opérateurs, etc.

    // Exemple simplifié:
    if (IsEOF())
    {
        return Token(TokenType::END, "", nbLigne, pos);
    }

    char c = CurrentChar();

    // Ignorer les espaces (mais pas les sauts de ligne qui sont gérés séparément)
    if (std::isspace(c) && c != '\n')
    {
        SkipWhitespace();
        return GetNextToken();
    }

    // Identifier ou mot-clé
    if (std::isalpha(c) || c == '_')
    {
        return ProcessIdentifier();
    }

    // Nombre
    if (std::isdigit(c))
    {
        return ProcessNumber();
    }

    // Chaîne
    if (c == '"' || c == '\'')
    {
        return ProcessString();
    }
    // std::cout << c << std::endl;
    switch (c)
    {
    case ';': pos++; return Token(TokenType::SEMICOLON, ";", nbLigne + 1, pos);
    case '(': pos++; return Token(TokenType::LPAREN, "(", nbLigne + 1, pos);
    case ')': pos++; return Token(TokenType::RPAREN, ")", nbLigne + 1, pos);
    case '+': pos++; return Token(TokenType::PLUS, "+", nbLigne + 1, pos);
    case '-': pos++; return Token(TokenType::MINUS, "-", nbLigne + 1, pos);
    case '*': pos++; return Token(TokenType::MULTIPLY, "*", nbLigne + 1, pos);
    case '/': pos++; return Token(TokenType::DIVIDE, "/", nbLigne + 1, pos);
    case ':': pos++; return Token(TokenType::COLON, ":", nbLigne + 1, pos);
    case '.': pos++; return Token(TokenType::DOT, ".", nbLigne + 1, pos);
    case ',': pos++; return Token(TokenType::COMMA, ",", nbLigne + 1, pos);
    case '[': pos++; return Token(TokenType::LBRACKET, "[", nbLigne + 1, pos);
    case ']': pos++; return Token(TokenType::RBRACKET, "]", nbLigne + 1, pos);

    case '=':
        if (NextChar() == '=') { pos++; return Token(TokenType::EQUAL_EQUAL, "==", nbLigne, pos); }
        break;

    case '>':
        if (NextChar() == '=') { pos++; return Token(TokenType::GREATER_EQUAL, ">=", nbLigne, pos); }
        return Token(TokenType::GREATER, ">", nbLigne + 1, pos);

    case '<':
        if (NextChar() == '=') { pos++; return Token(TokenType::LESS_EQUAL, "<=", nbLigne, pos); }
        if (contenu[pos] == '-') { pos++; return Token(TokenType::EQUAL, "<=", nbLigne, pos); }
        return Token(TokenType::LESS, "<", nbLigne + 1, pos);
    case '!':
        if (NextChar() == '=') { pos++; return Token(TokenType::NOT_EQUAL, "!=", nbLigne, pos); }
        break;

        // Token non reconnu
        std::string unknown(1, c);
        NextChar(); // Avancer
        return Token(TokenType::END, unknown, nbLigne, pos - 1);


    }
}


Token Lexer::ProcessNumber() 
{
    std::string value;
    while (!IsEOF() && std::isdigit(contenu[pos])) { value.push_back(CurrentChar()); NextChar(); }
    return Token(TokenType::NUMBER, value, nbLigne, pos);
}

Token Lexer::ProcessString()
{
    char quote = CurrentChar();
    std::string str;
    int startCol = pos + 1;

    NextChar(); // Skip the opening quote

    while (!IsEOF() && CurrentChar() != quote) {
        if (CurrentChar() == '\\') {
            NextChar(); // Skip the backslash
            if (!IsEOF()) {
                str += CurrentChar();
            }
        }
        else {
            str += CurrentChar();
        }
        NextChar();
    }

    if (CurrentChar() == quote) {
        NextChar(); // Skip the closing quote
    }
    else {
        // Handle unterminated string error
        std::cerr << "Error: Unterminated string at line " << nbLigne << std::endl;
    }

    return Token(TokenType::STRING, str, nbLigne, startCol);
}

Token Lexer::ProcessIdentifier()
{
    std::string value;
    while (!IsEOF() && std::isalpha(contenu[pos])) { value.push_back(CurrentChar()); NextChar(); } // la boucle va s'arreter au premier espace qu'elle voit
    if (value == "ENTIER") return Token(TokenType::INT, value, nbLigne, pos);
    if (value == "BOOLEAN") return Token(TokenType::BOOL, value, nbLigne, pos);
    if (value == "REEL") return Token(TokenType::FLOAT, value, nbLigne, pos);
    if (value == "STRING") return Token(TokenType::STRINGVAR, value, nbLigne, pos);
    if (value == "TRUE") return Token(TokenType::TRUE, value, nbLigne, pos);
    if (value == "FALSE") return Token(TokenType::FALSE, value, nbLigne, pos);
    if (value == "ecrire") return Token(TokenType::PRINT, value, nbLigne, pos);
    if (value == "lire") return Token(TokenType::LIRE, value, nbLigne, pos);
    if (value == "VARIABLE") return Token(TokenType::VARIABLE, value, nbLigne, pos);
    if (value == "DEBUT") return Token(TokenType::DEBUT, value, nbLigne, pos);
    if (value == "ET") return Token(TokenType::ET, value, nbLigne + 1, pos);
    if (value == "OU") return Token(TokenType::OU, value, nbLigne + 1, pos);
    if (value == "SI") return Token(TokenType::IF, value, nbLigne, pos);
    if (value == "SINON") return Token(TokenType::ELSE, value, nbLigne, pos);
    if (value == "FIN") return Token(TokenType::FIN, value, nbLigne, pos);
    if (value == "TANTQUE") return Token(TokenType::TANTQUE, value, nbLigne, pos);
    if (value == "FINTANTQUE") return Token(TokenType::FINTANTQUE, value, nbLigne, pos);
    if (value == "FINSI") return Token(TokenType::FINSI, value, nbLigne, pos);
    if (value == "POUR") return Token(TokenType::POUR, value, nbLigne, pos);
    if (value == "DE") return Token(TokenType::DE, value, nbLigne, pos);
    if (value == "A") return Token(TokenType::A, value, nbLigne, pos);
    if (value == "FINPOUR") return Token(TokenType::FINPOUR, value, nbLigne, pos);
    if (value == "TABLEAU") return Token(TokenType::TABLEAU, value, nbLigne, pos);
    return Token(TokenType::NAME, value, nbLigne, pos);
}


void Lexer::printTokens(std::vector<Token> tokenList)
{
    for (int i = 0; i < tokenList.size(); i++)
    {
        std::string s;
        switch (tokenList[i].type)
        {
        case TokenType::EQUAL: s = "Type: EQUALS, "; break;
        case TokenType::NAME: s = "Type: NAME, Value:" + tokenList[i].value; break;
        case TokenType::IF: s = "Type: IF"; break;
        case TokenType::ELSE: s = "Type: ELSE"; break;
        case TokenType::INDENT: s = "Type: INDENT"; break;
        case TokenType::DEDENT: s = "Type: DEDENT"; break;
        case TokenType::LPAREN: s = "Type: LPAREN"; break;
        case TokenType::NUMBER: s = "Type: NUMBER, " + tokenList[i].value; break;
        case TokenType::PLUS: s = "Type: PLUS"; break;
        case TokenType::RPAREN: s = "Type: RPAREN"; break;
        case TokenType::SEMICOLON: s = "Type: SEMICOLON"; break;
        case TokenType::MINUS: s = "Type: MINUS"; break;
        case TokenType::MULTIPLY: s = "Type: MULTIPLY"; break;
        case TokenType::DIVIDE: s = "Type: DIVIDE"; break;
        case TokenType::EQUAL_EQUAL: s = "Type: EQUAL_EQUAL"; break;
        case TokenType::COLON: s = "Type: COLON"; break;
        case TokenType::DOT: s = "Type: DOT"; break;
        case TokenType::COMMA: s = "Type: COMMA"; break;
        case TokenType::LESS: s = "Type: LESS"; break;
        case TokenType::LESS_EQUAL: s = "Type: LESS_EQUAL"; break;
        case TokenType::GREATER: s = "Type: GREATER"; break;
        case TokenType::GREATER_EQUAL: s = "Type: GREATER_EQUAL"; break;
        case TokenType::NOT_EQUAL: s = "Type: NOT_EQUAL"; break;
        case TokenType::TABLEAU: s = "Type: TABLEAU"; break;
        case TokenType::NEWLINE: s = "Type: NEWLINE"; break;
        case TokenType::INT: s = "Type: INT"; break;
        case TokenType::STRING: s = "Type: STRING, Value: " + tokenList[i].value; break;
        case TokenType::STRINGVAR: s = "Type: STRINGVAR"; break;
        case TokenType::DEBUT: s = "Type: DEBUT"; break;
        case TokenType::VARIABLE: s = "Type: VARIABLE"; break;
        case TokenType::PRINT: s = "Type: PRINT"; break;
        case TokenType::TANTQUE: s = "Type: TANTQUE"; break;
        case TokenType::POUR: s = "Type: POUR"; break;
        case TokenType::DE: s = "Type: DE"; break;
        case TokenType::A: s = "Type: A"; break;
        case TokenType::FINPOUR: s = "Type: FINPOUR"; break;
        case TokenType::LBRACKET: s = "Type: LBRACKET"; break;
        case TokenType::RBRACKET: s = "Type: RBRACKET"; break;
        case TokenType::FIN: s = "Type: FIN"; break;
        case TokenType::END: s = "Type: END"; break;
        default:
            s = "Token non implémenter dans le print Value: " + tokenList[i].value;
            break;
        }

        std::clog << "[" << i << "] " << s << std::endl;
    }
}