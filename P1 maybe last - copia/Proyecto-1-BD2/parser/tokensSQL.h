#ifndef TOKENS_SQL_H
#define TOKENS_SQL_H

#include <cctype>
#include <iostream>
#include <string>
#include <cstring>
#include <map>

using namespace std;

class Token {
public:
    enum Type {
        SELECT, CREATE, TABLE, FROM, ALL, WHERE, DELETE, EQUAL, BETWEEN, AND, INSERT, INTO, VALUES, FILE,
        LPARENT, RPARENT, INDEX, USING, AVL, SEQUENTIAL, EXTENDIBLE, END, ERR, SEMICOLON, COLON, ID, EOL, NUM, VALUE, QUOTE, FILENAME, TRUE, FALSE, FLOAT
    };
    static const char* token_names[34];
    Type type;
    string lexema;

    Token(Type);
    Token(Type, char c);
    Token(Type, const string& source);

    friend std::ostream& operator<<(std::ostream& outs, const Token& tok);
    friend std::ostream& operator<<(std::ostream& outs, const Token* tok);
};

const char* Token::token_names[34] = {
    "SELECT", "CREATE", "TABLE", "FROM", "ALL", "WHERE", "DELETE", "EQUAL", "BETWEEN", "AND", "INSERT", "INTO", "VALUES", "FILE",
    "LPARENT", "RPARENT", "INDEX", "USING", "AVL", "SEQUENTIAL", "EXTENDIBLE", "END", "ERR", "SEMICOLON", "COLON", "ID", "EOL", "NUM", "VALUE", "QUOTE", "FILENAME", "TRUE", "FALSE", "FLOAT"
};

Token::Token(Type type) : type(type) { lexema = ""; }
Token::Token(Type type, const string& source) : type(type) { lexema = source; }
Token::Token(Type type, char c) : type(type) { lexema = string(1, c); }

std::ostream& operator<<(std::ostream& outs, const Token& tok) {
    if (tok.lexema.empty())
        return outs << Token::token_names[tok.type];
    else
        return outs << Token::token_names[tok.type] << "(" << tok.lexema << ")";
}

std::ostream& operator<<(std::ostream& outs, const Token* tok) {
    return outs << *tok;
}

class ReservedWords {
    map<string, Token::Type> palabras;

public:
    ReservedWords();
    Token::Type search(const string& lexema);
};

ReservedWords::ReservedWords() {
    palabras.insert(make_pair("SELECT", Token::SELECT));
    palabras.insert(make_pair("CREATE", Token::CREATE));
    palabras.insert(make_pair("TABLE", Token::TABLE));
    palabras.insert(make_pair("FROM", Token::FROM));
    palabras.insert(make_pair("WHERE", Token::WHERE));
    palabras.insert(make_pair("DELETE", Token::DELETE));
    palabras.insert(make_pair("BETWEEN", Token::BETWEEN));
    palabras.insert(make_pair("AND", Token::AND));
    palabras.insert(make_pair("INSERT", Token::INSERT));
    palabras.insert(make_pair("INTO", Token::INTO));
    palabras.insert(make_pair("VALUES", Token::VALUES));
    palabras.insert(make_pair("FILE", Token::FILE));
    palabras.insert(make_pair("INDEX", Token::INDEX));
    palabras.insert(make_pair("USING", Token::USING));
    palabras.insert(make_pair("AVL", Token::AVL));
    palabras.insert(make_pair("SEQUENTIAL", Token::SEQUENTIAL));
    palabras.insert(make_pair("EXTENDIBLE", Token::EXTENDIBLE));
}

Token::Type ReservedWords::search(const string& lexema) {
    auto it = palabras.find(lexema);
    if (it != palabras.end()) return it->second;
    return Token::ID;
}

class Scanner {
public:
    Scanner(const char* in_s);
    Token* nextToken();
    ~Scanner();

private:
    string input;
    ReservedWords palabras;
    int first, current;
    bool isNumber(const string& s);
    char nextChar();
    void rollBack();
    void startLexema();
    string getLexema();
};

Scanner::Scanner(const char* s) : input(s), first(0), current(0) { }

Token* Scanner::nextToken() {
    Token* token;
    char c;

    // Consume whitespaces
    do {
        c = nextChar();
    } while (c == ' ' || c == '\t' || c == '\n');

    if (c == '\0') return new Token(Token::END);

    startLexema();
    if (isdigit(c) || isalpha(c) || c == '_') {
        c = nextChar();
        while (isalpha(c) || isdigit(c) || c == '_') {
            c = nextChar();
        }

        rollBack();
        string lexema = getLexema();
        Token::Type type = palabras.search(lexema);
        if (type != Token::ID) {
            return new Token(type, lexema);
        } else if (isNumber(lexema)) {
            return new Token(Token::NUM, lexema);
        } else {
            return new Token(Token::ID, lexema);
        }
    } else if (c == '\'') {
        // Handle filenames or quoted values
        startLexema();
        c = nextChar();
        while (c != '\'' && c != '\0') {
            c = nextChar();
        }
        if (c == '\'') {
            string lexema = getLexema();
            return new Token(Token::VALUE, lexema.substr(1, lexema.length() - 2)); // Exclude the starting and ending quote
        } else {
            return new Token(Token::ERR, c);
        }
    } else if (strchr("()=,;*'", c)) {
        switch (c) {
            case '(': return new Token(Token::LPARENT);
            case ')': return new Token(Token::RPARENT);
            case '=': return new Token(Token::EQUAL);
            case ',': return new Token(Token::COLON);
            case ';': return new Token(Token::SEMICOLON);
            case '*': return new Token(Token::ALL);
            case '\'': return new Token(Token::QUOTE);
            default: return new Token(Token::ERR, c);
        }
    } else {
        return new Token(Token::ERR, c);
    }
}

Scanner::~Scanner() { }

bool Scanner::isNumber(const string& s) {
    for (char c : s) {
        if (!isdigit(c)) {
            return false;
        }
    }
    return true;
}

char Scanner::nextChar() {
    if (current < input.length()) {
        return input[current++];
    } else {
        return '\0';
    }
}

void Scanner::rollBack() {
    if (current > 0) current--;
}

void Scanner::startLexema() {
    first = current - 1;
}

string Scanner::getLexema() {
    return input.substr(first, current - first);
}

#endif // TOKENS_SQL_H
