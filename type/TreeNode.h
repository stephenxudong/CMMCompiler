//
//  TreeNode.h
//  CMM
//
//  Created by Stephen Liao on 2018/10/9.
//  Copyright © 2018 Stephen. All rights reserved.
//

#ifndef TreeNode_h
#define TreeNode_h
#define MAXCHILDREN 50
#include "TreeNodeType.h"
#include "tokentype.h"
#include <vector>
#include <string>
using namespace TreeNodeType;
using std::vector;
class TreeNode{
public:
    vector<TreeNode*> child;
    //当前节点是表达式还是其他语句
    string nodekind;
    //内容
    string content;
    //行号
    int line;
    string type;
    string val;
public:
    TreeNode(string content):content(content){
           nodekind = "";
    }


    TreeNode(string nk, string content){
        this->nodekind = nk;

        this->content = content;
    }

    TreeNode(string nk, string content, int line){
        this->nodekind = nk;

        this->content = content;
        this->line = line;
    }
    
    /**
     构造字符值的构造函数
     @param nk nodekind
     @param type int or real
     @param val value
     */
     TreeNode(string nk,string type, string val):nodekind(nk),type(type),val(val){

     }

     inline void addChild(TreeNode* node){
         this->child.push_back(node);
     }

     inline TreeNode* getChild(int index){
         return this->child[index];
     }

     inline size_t childNum(){
         return this->child.size();
     }

     inline void remove(int index){
         this->child.erase(child.begin()+index);
     }
    
     string getContent() const;
     void setContent(const string &value);

     void releaseChild();
     string getNodekind() const;
     void setNodekind(const string &value);
     int getLine() const;
     void setLine(int value);
};
#endif /* TreeNode_h */




