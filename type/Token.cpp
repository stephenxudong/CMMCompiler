//
// Created by Stephen Liao on 2018/9/12.
//

#include "Token.h"
using std::string;

void Token::setText(string &text) {
    this->text = string(text);
}

void Token::setType(string &val) {
    this->type = string(val);
}

int Token::getLine() const {
    return line;
}

void Token::setLine(int line) {
    Token::line = line;
}

int Token::getPosition() const {
    return position;
}

void Token::setPosition(int position) {
    Token::position = position;
}


const string Token::getType() const {
    return type;
}

const string Token::getText() const {
    return text;
}

Token::Token(const string &type, const string &text,
             int line, int position)
        : type(type), text(text), line(line),
position(position) {}

std::string Token::toString() {
    string txt;
    if(!this->text.compare("\n"))
        txt = "\\n";
    else if(!this->text.compare("\t"))
        txt = "\\t";
    else if(!this->text.compare("\r"))
        txt = "\\r";
    else txt = this->text;
    string s = "";
    s
    .append("line: ").append(to_string(this->line))
    .append("   position: ").append(to_string(this->position))
    .append("   type: ").append(this->type)
    .append("   text: ").append(txt);
    
    return s;
}

