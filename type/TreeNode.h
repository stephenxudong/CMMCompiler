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
#include <vector>
#include <string>
using namespace TreeNodeType;

static int idx = 0;
class TreeNode{
public:
    vector<TreeNode*> child;
    int nodeNo;
    //当前节点是表达式还是其他语句
    TreeNodeK nodekind;
    string type;
    string val;
public:
//    TreeNode(NodeKind nk,StmtKind sk):nodekind(nk),sk(sk){}
//    TreeNode(NodeKind nk,ExpKind ek):nodekind(nk),ek(ek){}
//
//    TreeNode(NodeKind nk,Opkind opk):nodekind(nk),opk(opk){}
    TreeNode(TreeNodeK nk):nodekind(nk){
        this->nodeNo = idx++;
    }
    
    /**
     构造字符值的构造函数
     @param nk nodekind
     @param type int or real
     @param val value
     */
     TreeNode(TreeNodeK nk,string type, string val):nodekind(nk),type(type),val(val){
          this->nodeNo = idx++;
     }
//    TreeNode(NodeKind nk,ExpKind ek,string type, string val):nodekind(nk),ek(ek),type(type),val(val){}
    
    
};
#endif /* TreeNode_h */
