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
#include "type/TreeNodeType.h"
using namespace TreeNodeType;
using namespace std;
/**
 * 创建符号表,用于记录标识符的声明信息
 * 便于检查标识符是否重定义或者未定义使用
 * 在以后的语义分析中使用符号表的代表
 */

// 符号表的表节点

class TableNode{
public:
    int num;
    //元素的名字
    string name;
    //元素的类型
    string kind;
//    //是否声明,便于检查重定义或者未声明使用
//    bool isDecl;
    // 行号
    int line;
    //作用域
    int level;
    //可能variable类型的值
    string intVal;
    string realVal;
    string stringVal;
    //若是数组类型，其值是数组的大小，否则是0
    int arrayEleCnt;
public:
    TableNode(string name, string kind, int line, int level):
        name(name),kind(kind),line(line),level(level){
        intVal = "";
        realVal = "";
        stringVal = "";
        arrayEleCnt = 0;
    }

    string tostr() const;

    //需要使用重载的相等运算符
    bool operator==(const TableNode& node){
        return this->tostr()==node.tostr();
    }
    string getStringVal() const;
    void setStringVal(const string &value);
    string getKind() const;
    void setKind(const string &value);
    string getIntVal() const;
    void setIntVal(const string &value);
    string getRealVal() const;
    void setRealVal(const string &value);
    string getName() const;
    void setName(const string &value);
    int getLevel() const;
    void setLevel(int value);
};












#endif /* TableNode_h */
