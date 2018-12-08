#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <vector>
#include <algorithm>
#include "type/TableNode.h"
class SymbolTable{
private:
    static vector<TableNode*> table;

public:
    TableNode* get(int index){
        return table[index];
    }

    /**
     * @brief getAllLevel
     * 根据对象的名字对所有的作用域进行查找。没找到，返回空指针
     * @param name
     * @param level
     * @return
     */
    TableNode* getAllLevel(string name, int level);


    /**
     * @brief searchInLevel
     * 根据对象的名字在当前的作用域查找，没找到，返回空指针
     * @param name
     * @param level
     * @return
     */
    TableNode* searchInLevel(string name, int level);

    /**
     * @brief add
     * 在符号表中添加元素
     * @param node
     */
    inline void add(TableNode* node){
        table.push_back(node);
    }

    /**
     * @brief add
     * 在指定索引处添加
     * @param node
     * @param index
     */
    inline void add(TableNode* node,int index){
        table.insert(table.begin()+index,node);
    }

    /**
     * @brief remove
     * 删除符号表中的指定index的元素
     * @param index
     */
    inline void remove(int index){
        table.erase(table.begin()+index);
    }
    /**
     * @brief remove
     * 删除符号表中指定名字和作用域的元素
     * @param name
     * @param level
     */
    void remove(string name, int level);

    /**
     * @brief clear
     * 清空符号表
     */
    inline void clear(){
        table.clear();
    }

    /**
     * @brief update
     * 在作用域降低时更新符号表，删除无效元素
     * @param level
     */
    void update(int level){
        auto i = table.begin();
        while(i!=table.end()){
            if((*i)->getLevel()>level)
                i = table.erase(i);
            else i++;
        }
    }

    /**
     * @brief contains
     * 查找符号表中是否存在特定元素
     * @param node
     * @return
     */
    bool contains(const TableNode*& node){
        auto i = std::find(table.begin(),table.end(),node);
        if(i!=table.end())
            return true;
        return false;
    }


};

#endif // SYMBOLTABLE_H


