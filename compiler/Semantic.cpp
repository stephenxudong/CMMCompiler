#include "Semantic.h"


bool Semantic::matchInteger(const string &s) {
    // r2 为多0的情况，需要排除
    return regex_match(s, regex("^-?\\d+$"))
            && !regex_match(s, regex("^-?0{1,}\\d+$"));
}

bool Semantic::matchReal(const string &s) {
    // 需要排除小数点前面唯一的0超多两次的情况
    return regex_match(s, regex("^(-?[1-9]+\\d*)(\\.\\d+)$"))
            && !regex_match(s, regex("^(-?0{2,})(\\.\\d+)+$"));;
}

void Semantic::run()
{
    table.clear();
    statement(root);
}

void Semantic::statement(TreeNode *root)
{
    for(size_t i = 0; i < root->child.size(); i++){
        TreeNode* currentNode = root->child[i];
        string type = currentNode->nodekind;//int_val or


    }
}
