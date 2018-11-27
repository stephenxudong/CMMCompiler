#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "type/SymbolTable.h"
#include "type/TreeNode.h"
#include "util/Thread.h"
#include "type/TreeNodeType.h"
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
    virtual void run();

private:
    bool matchInteger(const string &s);
    bool matchReal(const string &s);


    void statement(TreeNode* root);
    void declare(TreeNode* node);
    void assign(TreeNode* node);
    void ifStmt(TreeNode* node);
    void whileStmt(TreeNode* node);
    void readStmt(TreeNode* node);
    void writeStmt(TreeNode* node);
    void condition(TreeNode* node);
    void expression(TreeNode* node);
    void array(TreeNode* node, int size);

    bool checkID(TreeNode* node, int level);


};

#endif // SEMANTIC_H
