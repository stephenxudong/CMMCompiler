#include "type/SymbolTable.h"


//静态成员需要在cpp初始化
vector<TableNode> SymbolTable::table;

TableNode *SymbolTable::searchInLevel(string name, int level){
    for(auto& node:table){
        if(node.getName()==name && node.getLevel()==level){
            return &node;
        }
    }
    return nullptr;
}

void SymbolTable::remove(string name, int level){
    auto iter = table.begin();
    while(iter != table.end()){
        if(iter->getName()==name && iter->getLevel()==level){
            iter = table.erase(iter);//删除操作会让迭代器失效
            return;
        }
        else iter++;
    }
}

TableNode *SymbolTable::getAllLevel(string name, int level){
    while(level>-1){
        for(auto& node:table){
            if(node.getName()==name && node.getLevel()==level){
                return &node;
            }
        }
        level--;
    }
    return nullptr;
}
