#include "Semantic.h"
#define NULL_STRING ""

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

void Semantic::error(string err, int line)
{
    string s  = "ERROR: 第";
    s.append(to_string(line)).append("行： ").append(err);
    this->errors.push_back(s);
}

void Semantic::statement(TreeNode *root)
{
    for(size_t i = 0; i < root->child.size(); i++){
        TreeNode* currentNode = root->child[i];
        string type = currentNode->nodekind;//int_val or
        if(type==TreeNodeType::declK)
            declare(currentNode);
        else if(type==TreeNodeType::operatorK && currentNode->content==TokenType::ASSIGN)
            assign(currentNode);
        else if(type==TreeNodeType::ForK){
            level++;
            forStmt(currentNode);
            level--;
            table.update(level);
        }
        else if(type==TreeNodeType::Ifk){
            level++;
            ifStmt(currentNode);
            level--;
            table.update(level);
        }
        else if(type==TreeNodeType::WhileK){
            level++;
            whileStmt(currentNode);
            level--;
            table.update(level);
        }
        else if(type==TreeNodeType::ReadK)
            readStmt(currentNode->getChild(0));
        else if(type==TreeNodeType::WriteK)
            writeStmt(currentNode->getChild(0));

    }
}

void Semantic::ifStmt(TreeNode *node)
{
    int count = node->childNum();
    TreeNode* conditionNode = node->getChild(0);
    TreeNode* stmtNode = node->getChild(1);
    if(condition(conditionNode->getChild(0))){
        statement(stmtNode);
    }else if(count==3){
        //条件为假且存在else
        TreeNode* elseNode = node->getChild(2);
        level++;
        statement(elseNode);
        level--;
        table.update(level);
    }else{
        //条件为假，不存在else
        return;
    }

}

void Semantic::forStmt(TreeNode *node)
{
    TreeNode* initNode = node->getChild(0);
    TreeNode* conditionNode = node->getChild(1);
    TreeNode* changeNode = node->getChild(2);
    TreeNode* stmtNode = node->getChild(3);
    //for循环初始化
    assign(initNode->getChild(0));
    while(condition(conditionNode->getChild(0))){
        statement(stmtNode);
        level--;
        table.update(level);
        level++;
        assign(changeNode->getChild(0));

    }
}

void Semantic::whileStmt(TreeNode *node)
{
    TreeNode* conditionNode = node->getChild(0);
    TreeNode* stmtNode = node->getChild(1);
    while(condition(conditionNode->getChild(0))){
        statement(stmtNode);
        level--;
        table.update(level);
        level++;

    }
}

/**
 * @brief Semantic::condition
 * 分析if和while中的条件语句
 * @param node
 * @return
 */
bool Semantic::condition(TreeNode *node)
{
    string kind = node->getNodekind();
    string content = node->getContent();
    if(kind==TokenType::LITERAL_TRUE)
        return true;
    else if(kind==TokenType::LITERAL_FALSE)
        return false;
    else if(kind==TreeNodeType::IdentifierK){
        if(checkID(node,level)){
            if(node->childNum()!=0){
                TableNode* tn = table.getAllLevel(content,level);
                string s = array(node->getChild(0),tn->getArrayEleCnt());
                if(s != NULL_STRING)
                    content.append("@").append(s);
                else return false;
            }
            TableNode* tmp = table.getAllLevel(content,level);
            if(tmp->getKind()==TokenType::BOOL){
                if(tmp->getStringVal()==TokenType::LITERAL_FALSE)
                    return false;
                else return true;
            }else{
                string err = "不能将变量";
                err.append(content).append("作为判断条件");
                error(err,node->getLine());
            }

        }else{
            return false;
        }
    }
    else if(content==TokenType::EQUAL || content==TokenType::NEQUAL
            || content==TokenType::LT || content==TokenType::GT){
        //存放两个待比较对象的值
        string results[2];
        for(int i = 0; i < node->childNum(); i++){
            string kind = node->getChild(i)->getNodekind();
            string tempContent = node->getChild(i)->getContent();
            if(kind=="整数" || kind=="实数"){
                results[i] = tempContent;
            }
            else if(kind==TreeNodeType::IdentifierK){
                if(checkID(node->getChild(i),level)){
                    if(node->getChild(i)->childNum()!=0){
                        TableNode* tn = table.getAllLevel(tempContent,level);
                        string s = array(node->getChild(0),tn->getArrayEleCnt());
                        if(s != NULL_STRING)
                            content.append("@").append(s);
                        else return false;
                    }
                    TableNode* tmp = table.getAllLevel(tempContent,level);
                    if(tmp->getKind()==TokenType::LITERAL_INT)
                        results[i] = tmp->getIntVal();
                    else
                        results[i]= tmp->getRealVal();
                }else{
                    return false;
                }

            }
            else if(tempContent==TokenType::PLUS || tempContent==TokenType::MINUS
                    || tempContent==TokenType::MULTI || tempContent==TokenType::DIVIDE){
                string result = expression(node->getChild(i));
                if(result!=NULL_STRING)
                    results[i]=result;
                else
                    return false;
            }

        }
        if(results[0]!=NULL_STRING && results[1]!=NULL_STRING){
            double ele1 = atof(results[0].c_str());
            double ele2 = atof(results[1].c_str());
            if(content==TokenType::GT ){
                if(ele1 > ele2)
                    return true;
            }
            else if(content==TokenType::LT){
                if(ele1 < ele2)
                    return true;
            }
            else if(content==TokenType::EQUAL){
                if(results[0]==results[1])
                    return true;
            }
            else{
                if(results[0]!=results[1]){
                    return true;
                }
            }

        }
    }
    //语义分析出错或者分析条件为假
    return false;
}
/**
 * @brief Semantic::expression
 * 出错返回 ""
 * @param node
 * @return
 */
string Semantic::expression(TreeNode *node)
{
    return NULL_STRING;
}

/**
 * @brief Semantic::array
 * 出错返回空字符串""
 * @param node
 * @param size
 * @return
 */
string Semantic::array(TreeNode *node, int size)
{
    if(node->getNodekind()=="整数"){
        int i = atoi(node->getContent().c_str());
        if(i>-1 && i<size){
            return node->getContent();
        }
        else if(i<0){
            string err = "数组下标不能是负数";
            error(err,node->line);
            return NULL_STRING;
        }
        else{
            string err = "数组下表越界";
            error(err,node->line);
            return NULL_STRING;
        }
    }
    else if(node->getNodekind()==TreeNodeType::IdentifierK){
        if(checkID(node,level)){
            TableNode* temp = table.getAllLevel(node->getContent(),level);
            if(temp->getKind()==TokenType::LITERAL_INT){
                int i = atoi(temp->getIntVal().c_str());
                if(i>-1 && i <size)
                    return temp->getIntVal();
                else if(i<0){
                    string err = "数组下标不能是负数";
                    error(err,node->line);
                    return NULL_STRING;
                }
                else{
                    string err = "数组下表越界";
                    error(err,node->line);
                    return NULL_STRING;
                }
            }
            else{
                string err = "数组索引必须是整数";
                error(err,node->getLine());
                return NULL_STRING;
            }

        }
        else{
            return NULL_STRING;
        }
    }
    else if(node->getContent()==TokenType::PLUS || node->getContent()==TokenType::MINUS
            || node->getContent()==TokenType::MULTI || node->getContent()==TokenType::DIVIDE){
        string result = expression(node);
        if(result!=NULL_STRING){
            if(matchInteger(result)){
                int i = atoi(result.c_str());
                if(i>-1 && i <size)
                    return result;
                else if(i<0){
                    string err = "数组下标不能是负数";
                    error(err,node->line);
                    return NULL_STRING;
                }
                else{
                    string err = "数组下表越界";
                    error(err,node->line);
                    return NULL_STRING;
                }
            }
            else{
                string err = "数组索引必须是整数";
                error(err,node->getLine());
                return NULL_STRING;
            }
        }
        else
            return NULL_STRING;
    }
    return NULL_STRING;
}
