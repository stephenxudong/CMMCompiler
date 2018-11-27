#include "TreeNode.h"

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
