//
//  Table.h
//  CMM
//
//  Created by Stephen Liao on 2018/10/9.
//  Copyright © 2018 Stephen. All rights reserved.
//

#ifndef TableNode_h
#define TableNode_h
#include <string>
using std::string;
/**
 * 创建符号表,用于记录标识符的声明信息
 * 便于检查标识符是否重定义或者未定义使用
 * 在以后的语义分析中使用符号表的代表
 */

// 符号表的表节点
class TableNode{
public:
    int num;
    string str;
    bool isDecl;//是否声明,便于检查重定义或者未声明使用
    TableNode* next;

public:
    TableNode():num(0), str(""), isDecl(false), next(nullptr){}
    TableNode(string& s, bool isDecl=false, TableNode* next=nullptr){
        this->str = s;
        this->isDecl = isDecl;
        this->next = next;
    }
};
//extern TableNode* head;

#endif /* TableNode_h */
