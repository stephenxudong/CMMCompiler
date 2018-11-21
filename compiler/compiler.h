//
// Created by Stephen Liao on 2018/9/19.
//

#ifndef CMMINTEPRETER_COMPILER_H
#define CMMINTEPRETER_COMPILER_H

#define MAXIDNUM 10000
#include <vector>
#include <map>
#include "Lexical.h"
#include "Parser.h"
#include <iostream>
#include <fstream>
#include "../util/Util.h"
namespace compiler{
    //static TableNode *head = new TableNode();//符号表
    vector<Token> lexcial(const char* filepath,const char* output=nullptr);
    TreeNode* parse(vector<Token>& tokens,char* output=nullptr);
}


#endif //CMMINTEPRETER_COMPILER_H
