#include "TreeNode.h"

string TreeNode::getNodekind() const
{
    return nodekind;
}

void TreeNode::setNodekind(const string &value)
{
    nodekind = value;
}

int TreeNode::getLine() const
{
    return line;
}

void TreeNode::setLine(int value)
{
    line = value;
}

std::string TreeNode::getContent() const
{
    return content;
}

void TreeNode::setContent(const std::string &value)
{
    content = value;
}

void TreeNode::releaseChild(){
    if(this->child.size()!=0){
        for(int i = 0; i < this->child.size(); i++){
            child[i]->releaseChild();
            delete child[i];
        }
    }

}
