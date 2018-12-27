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
#include "Semantic.h"
#include <iostream>
#include <fstream>
#include "mainwindow.h"
#include "../util/Util.h"
#include <QWaitCondition>
namespace Compiler{
    static QWaitCondition semaphore;
    static bool continueFlag = true;
    void compile(const char* filepath,MainWindow& w,const char* output=nullptr);

    vector<Token> lexcial(vector<string>& lines,vector<string>& errors);
    TreeNode* parse(vector<Token>& tokens,vector<string>& errors);
    void semantic(TreeNode* root,MainWindow& w);
}




#endif //CMMINTEPRETER_COMPILER_H
