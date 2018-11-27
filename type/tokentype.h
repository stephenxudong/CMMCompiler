//
//  tag.h
//  CMM
//
//  Created by Stephen Liao on 2018/10/1.
//  Copyright © 2018 Stephen. All rights reserved.
//

#ifndef tag_h
#define tag_h
#include <string>
using std::string;
namespace TokenType {
    static const string KEYWORD = "keywords";
    static const string IDENTIFIER = "identifier";
    
    static const string PLUS = "+";
    static const string MINUS = "-";
    static const string DIVIDE = "/";
    static const string MULTI = "*";
    static const string MOD = "%";
    static const string LT = "<";
    static const string GT = ">";
    static const string GET = ">=";
    static const string LET = "<=";
    static const string EQUAL = "==";
    static const string NEQUAL = "<>";
    static const string ASSIGN = "=";
    
    /* 保留字 */
    
    static const string INT = "int";
    static const string REAL = "real";
    static const string READ = "read";
    static const string WRITE = "write";
    static const string WHILE = "while";
    static const string IF = "if";
    static const string FOR = "for";
    static const string ELSE = "else";
    static const string BOOL = "bool";
    static const string STRING = "string";

    
    
    /**/
    static const string LITERAL_INT = "int_val";
    static const string LITERAL_REAL = "real_val";
    static const string LITERAL_STRING = "string_val";
    static const string LITERAL_TRUE = "true";
    static const string LITERAL_FALSE = "false";
    /* 分隔符*/
    static const string DQ = "\"";
    static const string RBRACE = "}";
    static const string LBRACE = "{";
    static const string RPAREN = ")";
    static const string LPAREN = "(";
    static const string RBRACKET = "]";
    static const string LBRACKET = "[";
    static const string COMMA = ",";
    static const string SEMICOLON = ";";
    
    /* 注释符*/
    static const string ROW = "//";
    static const string LEFT = "/*";
    static const string RIGHT = "*/";
    
    /* 错误*/
    static const string ERROR = "错误    ： ";
}

#endif /* tag_h */
