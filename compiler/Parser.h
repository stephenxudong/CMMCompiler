//
//  Parser.h
//  CMM
//
//  Created by Stephen Liao on 2018/10/9.
//  Copyright © 2018 Stephen. All rights reserved.
//

#ifndef Parser_h
#define Parser_h
#include <vector>
#include "../type/Token.h"
#include "../type/TreeNode.h"
#include <stdlib.h>
#include <iostream>
class Parser{
private:
    vector<Token> tokens;//删除了换行符，制表符，多行注释
    int nodeNo;
    int index; // 当前的token数
    TreeNode* root;
    Token curr;//需要用初始化列表初始化
    int errnum;
    vector<string> errors;
    
public:
    Parser(vector<Token>& tokens)
    :tokens(tokens),curr(Token("","",0,0)){
        this->root = new TreeNode(TreeNodeType::Programk,"Program");
        this->index = -1;
        this->nodeNo = 1;
    }
    
    TreeNode* parse();
    void printT(TreeNode* t,char* outpath=nullptr);
    vector<string> error() const{
        return errors;
    }
    void printTree(TreeNode* root, int depth);
    
private:
    
    inline bool hasNextToken(){
        return this->index+1 < this->tokens.size();
    }
    inline void nextToken(){
        if(index==tokens.size()-1){
            return;
        }
        curr = tokens[++index];
    }
    bool consumeNextToken(string type);
    inline string getNextTokenType(){
        return tokens.at(index+1).getType();
    }

    void error(string error);

    void checkCurrentType(const string& type, TreeNode* node=nullptr);
    //比较下一下参数的类型和预定的
    bool checkNextTokenType(int n,...);
    bool checkNextTokenType(vector<string> types);
    TreeNode* parseStmt();
    TreeNode* parseForStmt();
    TreeNode* parseIfStmt();
    TreeNode* parseWhileStmt();
    TreeNode* parseReadStmt();
    TreeNode* parseWriteStmt();
    TreeNode* parseAssignStmt(bool inFor=false);
    TreeNode* parseDeclStmt();
    TreeNode* delc_aid(TreeNode* &node);
    TreeNode* parseExp();
    TreeNode* add_op();
    TreeNode* parseConditionExp();
    TreeNode* comparison_op();
    TreeNode* term();
    TreeNode* mul_op();
    TreeNode* factor();
    TreeNode* array();
};

#endif /* Parser_h */
