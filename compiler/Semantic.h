#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "type/SymbolTable.h"
#include "type/TreeNode.h"
#include "util/Thread.h"
#include <regex>

class Semantic : public Thread
{
private:
    SymbolTable table;//符号表
    TreeNode* root;//抽象语法树
    int level;//语义分析标识符的作用域
    //错误处理
    string errorInfo;
    int errorNum = 0;
    vector<string> errors;
    string userInput;

public:
    Semantic(TreeNode* root):root(root){
        errorInfo = "";
        userInput = "";
    }
    bool matchInteger(const string &s);
    bool matchReal(const string &s);
    virtual void run();
    void statement(TreeNode* root);
};

#endif // SEMANTIC_H
