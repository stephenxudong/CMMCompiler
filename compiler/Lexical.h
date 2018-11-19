//
// Created by Stephen Liao on 2018/9/19.
//

#ifndef CMMINTEPRETER_LEXICAL_H
#define CMMINTEPRETER_LEXICAL_H


#include "../type/Token.h"
#include <stdlib.h>
#include <stdio.h>
#include <regex>
#include <cctype>
class Lexical {
private:
    int line;
    vector<Token> tokens;
    vector<string> errors;
    int errnum;
    bool comment;
private:
    static inline bool isLetter(const char c){
        return isalpha(c) || c == '_';
    }
    static inline bool isDigit(const char c){
        return c>='0' && c<='9';
    }
    static bool matchInteger(const string& s);
    static bool matchReal(const string &s);
    static bool mathchIdentifier(const string& s);
    static bool matchKey(const string& s);
    static inline bool validate(const char ch);
    static inline string valueOf(const char c){
        return string(1,c);
    }
private:
    void parseLine(const string& line, int lineNum);
public:
    Lexical():errnum(0),comment(false){}
    void parse(const vector<string>& lines);
    vector<string> geterrors() const{
        return this->errors;
    }
    vector<Token> getTokens() const{
        return this->tokens;
    }
    
};


#endif //CMMINTEPRETER_LEXICAL_H
