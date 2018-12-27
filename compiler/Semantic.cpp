#include "Semantic.h"
#include <math.h>
#include "compiler/compiler.h"
#include <QString>
#define NULL_STRING ""

extern QWaitCondition semaphore;
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

void Semantic::setUserInput(const string &value)
{
    userInput = value;
}

Semantic::Semantic(TreeNode *root, MainWindow &w):w(&w){
    this->root = root;
    errorInfo = "";
    userInput = "";
}

void Semantic::run()
{
    table.clear();
    statement(root);
}

void Semantic::pause()
{
    this->m_mutex.lock();
    this->flag = false;
    this->m_mutex.unlock();
}

void Semantic::resume()
{
    this->m_mutex.lock();
    this->flag = true;
    this->m_mutex.unlock();
}

void Semantic::error(string err, int line)
{
    string s  = "ERROR: 第";
    s.append(to_string(line)).append("行： ").append(err);
    //语义错误直接输出
    QString qs;
    w->getTextEdit()->append(qs.fromStdString(s).append("\n"));
//    cout<<s<<endl;
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

void Semantic::declare(TreeNode *node)
{
    //decl node: nodekind:declK,content: int | real | bool | string
    //当前节点所包含要声明的白变量的类型
    string content = node->getContent();
    int idx = 0;
    while(idx<node->childNum()){
        TreeNode* temp = node->getChild(idx);
        //变量名字
        string name = temp->getContent();
        if(table.searchInLevel(name,level)==nullptr){
            //声明普通变量
            if(temp->childNum()==0){
                TableNode* element = new TableNode(temp->getContent(),content,temp->getLine(),level);
                idx++;
                //判断变量在声明的时候有没有初始化
                if(idx < node->childNum() && node->getChild(idx)->getContent()==TokenType::ASSIGN){
                    TreeNode* valNode = node->getChild(idx)->getChild(0);
                    string value = valNode->getContent();
                    /** declear int variable*/
                    if(content==TokenType::INT){
                        //declear int variable
                        if(matchInteger(value)){
                            element->setIntVal(value);
                            element->setRealVal(to_string(atof(value.c_str())));
                        }
                        else if(matchReal(value)){
                            string err = "value和类型不匹配:浮点数不能复制给int型变量";
                            error(err,valNode->getLine());
                        }
                        else if(value==TokenType::LITERAL_FALSE || value == TokenType::LITERAL_TRUE){
                            string err = "value和类型不匹配:布尔值不能复制给int型变量";
                            error(err,valNode->getLine());
                        }
                        else if(valNode->getNodekind()=="字符串"){
                            string err = "value和类型不匹配:字符串不能复制给int型变量";
                            error(err,valNode->getLine());
                        }
                        else if(valNode->getNodekind()==TreeNodeType::IdentifierK){
                            if(checkID(valNode,level)){
                                if(table.getAllLevel(valNode->getContent(),level)->getKind()
                                        =="int"){
                                    element->setIntVal(table.getAllLevel(valNode->getContent(),level)->getIntVal());
                                    element->setRealVal(table.getAllLevel(valNode->getContent(),level)->getRealVal());
                                }
                                else if(value=="real"){
                                    string err = "value和类型不匹配:浮点数不能复制给int型变量";
                                    error(err,valNode->getLine());
                                }
                                else if(value==TokenType::LITERAL_FALSE || value == TokenType::LITERAL_TRUE){
                                    string err = "value和类型不匹配:布尔值不能复制给int型变量";
                                    error(err,valNode->getLine());
                                }
                                else if(valNode->getNodekind()=="字符串"){
                                    string err = "value和类型不匹配:字符串不能复制给int型变量";
                                    error(err,valNode->getLine());
                                }
                            }
                            else{
                                return;
                            }
                        }
                        else if(value==TokenType::PLUS || value==TokenType::MINUS
                                ||value==TokenType::MULTI||value==TokenType::DIVIDE){
                            string result = expression(valNode);
                            if(result!=NULL_STRING){
                                if(matchInteger(result)){
                                    element->setIntVal(result);
                                    element->setRealVal(to_string(atof(result.c_str())));
                                }
                                else if(matchReal(result)){
                                    string err = "value和类型不匹配:浮点数不能复制给int型变量";
                                    error(err,valNode->getLine());
                                    return;
                                }
                                else return;

                    }

                            }
                        }
                    /** declear real variable*/
                    else if(content==TokenType::REAL){
                        if(matchInteger(value)){
                            element->setRealVal(to_string(atof(value.c_str())));
                        }
                        else if(matchReal(value)){
                            element->setRealVal(value);
                        }
                        else if(value==TokenType::LITERAL_FALSE || value == TokenType::LITERAL_TRUE){
                            string err = "value和类型不匹配:布尔值不能复制给real型变量";
                            error(err,valNode->getLine());
                        }
                        else if(valNode->getNodekind()=="字符串"){
                            string err = "value和类型不匹配:字符串不能复制给real型变量";
                            error(err,valNode->getLine());
                        }
                        else if(valNode->getNodekind()==TreeNodeType::IdentifierK){
                            if(checkID(valNode,level)){
                                if(table.getAllLevel(valNode->getContent(),level)->getKind()==TokenType::LITERAL_INT
                                || table.getAllLevel(valNode->getContent(),level)->getKind()
                                        ==TokenType::LITERAL_REAL){
                                    element->setRealVal(table.getAllLevel(valNode->getContent(),level)->getRealVal());
                                }
                                /* need to modify here!!!!!
                                 * need to modify here!!!!!
                                 * need to modify here!!!!!
                                 * need to modify here!!!!!
                                */
                                else if(table.getAllLevel(valNode->getContent(),level)->getKind()=="布尔值"){
                                    string err = "value和类型不匹配:浮点数不能复制给int型变量";
                                    error(err,valNode->getLine());
                                }
                                else if(value==TokenType::LITERAL_FALSE || value == TokenType::LITERAL_TRUE){
                                    string err = "value和类型不匹配:布尔值不能复制给int型变量";
                                    error(err,valNode->getLine());
                                }
                                else if(valNode->getNodekind()=="字符串"){
                                    string err = "value和类型不匹配:字符串不能复制给int型变量";
                                    error(err,valNode->getLine());
                                }
                            }
                            else{
                                return;
                            }
                        }
                        else if(value==TokenType::PLUS || value==TokenType::MINUS
                                ||value==TokenType::MULTI||value==TokenType::DIVIDE){
                            string result = expression(valNode);
                            if(result!=NULL_STRING){
                                if(matchInteger(result)){
                                    element->setRealVal(to_string(atof(result.c_str())));
                                }
                                else if(matchReal(result)){
                                    element->setRealVal(result);
                                }
                                else return;
                            }
                        }
                   }
                    /** declear string variable*/
                    else if(content==TokenType::STRING){
                        if(matchInteger(value)){
                            string err = "value和类型不匹配:int不能复制给string型变量";
                            error(err,valNode->getLine());
                        }
                        else if(matchReal(value)){
                            string err = "value和类型不匹配:real不能复制给string型变量";
                            error(err,valNode->getLine());
                        }
                        else if(value==TokenType::LITERAL_FALSE || value == TokenType::LITERAL_TRUE){
                            string err = "value和类型不匹配:int不能复制给string型变量";
                            error(err,valNode->getLine());
                        }
                        else if(valNode->getNodekind()=="字符串"){
                           element->setStringVal(value);
                        }
                        else if(valNode->getNodekind()==TreeNodeType::IdentifierK){
                            if(checkID(valNode,level)){
                                if(table.getAllLevel(valNode->getContent(),level)->getKind()==TokenType::LITERAL_INT){
                                    string err = "value和类型不匹配:int不能复制给string型变量";
                                    error(err,valNode->getLine());
                                }
                                else if(table.getAllLevel(valNode->getContent(),level)->getKind()==TokenType::LITERAL_REAL){
                                        string err = "value和类型不匹配:reals不能复制给string型变量";
                                        error(err,valNode->getLine());
                                }
                                else if(value==TokenType::LITERAL_FALSE || value == TokenType::LITERAL_TRUE){
                                    string err = "value和类型不匹配:布尔值不能复制给int型变量";
                                    error(err,valNode->getLine());
                                }
                                else if(table.getAllLevel(valNode->getContent(),level)->getKind()=="字符串"){
                                   element->setStringVal(value);
                                }
                            }
                            else{
                                return;
                            }
                        }
                        else if(value==TokenType::PLUS || value==TokenType::MINUS
                                ||value==TokenType::MULTI||value==TokenType::DIVIDE){
                            string err = "不能把算数表达式复制给字符串变量";
                            error(err,valNode->getLine());
                        }
                    }
                    /** declear bool variable*/
                    else{
                        if(matchInteger(value)){
                            int i = atoi(value.c_str());
                            if(i<=0)
                                element->setStringVal("false");
                            else
                                element->setStringVal("true");
                        }
                        else if(matchReal(value)){
                            string err = "value和类型不匹配:real不能复制给bool型变量";
                            error(err,valNode->getLine());
                        }
                        else if(value==TokenType::LITERAL_FALSE||value==TokenType::LITERAL_TRUE){
                           element->setStringVal(value);
                        }
                        else if(valNode->getNodekind()=="字符串"){
                            string err = "value和类型不匹配:字符串不能复制给bool型变量";
                            error(err,valNode->getLine());
                        }
                        else if(valNode->getNodekind()==TreeNodeType::IdentifierK){
                            if(checkID(valNode,level)){
                                if(table.getAllLevel(valNode->getContent(),level)->getKind()==TokenType::LITERAL_INT){
                                    int i = atoi(table.getAllLevel(valNode->getContent(),level)->getIntVal().c_str());
                                    if(i<=0)
                                        element->setStringVal("false");
                                    else
                                        element->setStringVal("true");
                                }
                                else if(table.getAllLevel(valNode->getContent(),level)->getKind()==TokenType::LITERAL_REAL){
                                    string err = "value和类型不匹配:real不能复制给bool型变量";
                                    error(err,valNode->getLine());
                                }
                                else if(table.getAllLevel(valNode->getContent(),level)->getKind()=="BOOL"){
                                    element->setStringVal(table.getAllLevel(valNode->getContent(),level)->getStringVal());
                                }
                                else if(table.getAllLevel(valNode->getContent(),level)->getKind()==TokenType::LITERAL_STRING){
                                    string err = "value和类型不匹配:string不能复制给bool型变量";
                                    error(err,valNode->getLine());
                                }
                                else if(value==TokenType::PLUS || value==TokenType::MINUS
                                        ||value==TokenType::MULTI||value==TokenType::DIVIDE){
                                    bool result = condition(valNode);
                                    if(result)
                                        element->setStringVal("true");
                                    else
                                        element->setStringVal("false");
                                }
                            }
                            else
                                return;
                        }

                    }
                    idx++;
                }

                table.add(element);
             }
            else{
                /**declear a array*/
                TableNode* element = new TableNode(temp->content,content,temp->getLine(),level);
                string sizeVal = temp->getChild(0)->getContent();
                if(matchInteger(sizeVal)){
                    int i = atoi(sizeVal.c_str());
                    if(i<1){
                        string err = "数组大小必须大于0";
                        error(err,node->getLine());
                        return;
                    }


                }
                else if(temp->getChild(0)->getNodekind()==TreeNodeType::IdentifierK){
                    if(checkID(node,level)){
                        TableNode* tempElement = table.getAllLevel(node->getContent(),level);
                        if(tempElement->getKind()==TokenType::LITERAL_INT){
                            int i = atoi(tempElement->getIntVal().c_str());
                            if(i<1){
                                string err = "数组大小必须大于0";
                                error(err,node->getLine());
                                return;
                            }
                            else
                                sizeVal = tempElement->getIntVal();
                        }
                        else{
                            string err = "类型不匹配,数组大小必须为整数类型";
                            error(err, node->getLine());
                            return;
                        }
                    }
                    else return;
                }
                else if(sizeVal==TokenType::PLUS || sizeVal==TokenType::MINUS
                        ||sizeVal==TokenType::MULTI||sizeVal==TokenType::DIVIDE){
                    sizeVal = expression(temp->getChild(0));
                    if(sizeVal!=NULL_STRING){
                        if(matchInteger(sizeVal)){
                            int i = atoi(sizeVal.c_str());
                            if(i<1){
                                string err = "数组大小必须大于0";
                                error(err,node->getLine());
                                return;
                            }
                        }
                        else{
                            string err = "数组大小必须是整形";
                            error(err,node->getLine());
                        }
                    }
                    else return;
                }
                element->setArrayEleCnt(atoi(sizeVal.c_str()));
                table.add(element);
                idx++;
                for(int j = 0; j < atoi(sizeVal.c_str());j++){
                    string s = temp->getContent();
                    s.append("@").append(to_string(j));
                    TableNode* ste = new TableNode(s,content,temp->getLine(),level);
                    table.add(ste);
                }

            }


        }
        else{
            string err = "变量重复声明";
            error(err,node->getLine());
            return;
        }

    }
}

void Semantic::assign(TreeNode *node)
{
    //赋值语句左边部分
    TreeNode* node1 = node->getChild(0);
    //左半部分标识符
    string node1Val = node1->getContent();
    if(table.getAllLevel(node1Val,level)!=nullptr){
        if(node1->childNum()!=0){
            string s = array(node1->getChild(0),table.getAllLevel(node1Val,level)->getArrayEleCnt());
            if(s!=NULL_STRING)
                node1Val.append("@").append(s);
            else return;
        }
    }
    else{
        string err = "变量未定义";
        error(err,node->getLine());
    }
    //赋值语句左半部分类型
    string node1kind = table.getAllLevel(node1Val,level)->getKind();
    //赋值语句右半部分
    TreeNode* node2 = node->getChild(1);
    string node2kind = node2->getNodekind();
    string node2Val = node2->getContent();
    //赋值语句右半部分的值
    string value = "";
    if(node2kind=="整数"){
        value = node2Val;
        node2kind = "int";
    }
    else if(node2kind == "实数"){
        value = node2Val;
        node2kind = "real";
    }
    else if(node2kind=="字符串"){
        value = node2Val;
        node2kind = "string";
    }
    else if(node2kind=="布尔值"){
        value = node2Val;
        node2kind = "bool";
    }
    else if(node2kind==TreeNodeType::IdentifierK){
        if(checkID(node2,level)){
            if(node2->childNum()!=0){
                string s = array(node2->getChild(0),table.getAllLevel(node2Val,level)->getArrayEleCnt());
                if(s!=NULL_STRING)
                    node2Val.append("@").append(s);
                else return;
            }
            TableNode* temp = table.getAllLevel(node2Val,level);
            if(temp->getKind()=="int"){
                value = temp->getIntVal();
            }
            else if(temp->getKind()=="real"){
                value = temp->getRealVal();
            }
            else if(temp->getKind()=="bool"||temp->getKind()=="string"){
                value = temp->getStringVal();
            }
            node2kind = temp->getKind();
        }
        else return;
    }

    else if(node2Val==TokenType::PLUS||node2Val==TokenType::MINUS
            ||node2Val==TokenType::MULTI||node2Val==TokenType::DIVIDE){

        string result = expression(node2);
        if (result != NULL_STRING) {
            if (matchInteger(result))
                node2kind = "int";
            else if (matchReal(result))
                node2kind = "real";
            value = result;
        } else {
            return;
        }
    }
    else if(node2Val==TokenType::EQUAL||node2Val==TokenType::NEQUAL
            ||node2Val==TokenType::LT||node2Val==TokenType::GT){
        bool result = condition(node2);
        node2kind = "bool";
        value = to_string(result);
    }

    //接下来在符号表中更新变量的值
    auto assnode = table.getAllLevel(node1Val,level);
    if(node1kind=="int"){
        if(node2kind=="int"){
            assnode->setIntVal(value);
            assnode->setRealVal(to_string(atof(value.c_str())));
        }
        else{
            string err = "赋值符号左右两侧类型不相同";
            error(err,node1->getLine());
        }

    }
    else if(node1kind=="real"){
        if(node2kind=="int"){
            assnode->setRealVal(to_string(atof(value.c_str())));
        }
        else if(node2kind=="real"){
            assnode->setRealVal(value);
        }
        else{
            string err = "赋值符号左右两侧类型不相同";
            error(err,node1->getLine());
        }
    }
    else if(node1kind=="bool"){
        if(node2kind == "int"){
            int i = atoi(value.c_str());
            if(i<0)
                assnode->setStringVal("false");
            else
                assnode->setStringVal("true");
        }
        else if(node2kind=="bool"){
            assnode->setStringVal(value);
        }
        else{
            string err = "赋值符号左右两侧类型不相同";
            error(err,node1->getLine());
        }
    }
    else if(node1kind=="string"){
        if(node2kind=="string"){
            assnode->setStringVal(value);
        }
        else{
            string err = "赋值符号左右两侧类型不相同";
            error(err,node1->getLine());
        }
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

void Semantic::readStmt(TreeNode *node)
{
    //要读取得变量的名字
    string idName = node->getContent();
    TableNode* element = table.getAllLevel(idName,level);
    if(element!=nullptr){
        if(node->childNum()!=0){
            string s = array(node->getChild(0),element->getArrayEleCnt());
            if(s!=NULL_STRING)
                idName.append("@").append(s);
            else return;
        }
        string value = readInput();
        if(element->getKind()=="int"){
            if(matchInteger(value)){
                table.getAllLevel(idName,level)->setIntVal(value);
                table.getAllLevel(idName,level)->setRealVal(to_string(atof(value.c_str())));
            }
            else{
                string err = "不能把\"";
                err.append(value).append("\"赋值给变量").append(idName);
                error(err,node->getLine());
            }
        }
        else if(element->getKind()=="real"){
            if(matchInteger(value)){
                table.getAllLevel(idName,level)->setRealVal(value);
            }
            else if(matchReal(value)){
                table.getAllLevel(idName,level)->setRealVal(to_string(atoi(value.c_str())));
            }
            else{
                string err = "不能把\"";
                err.append(value).append("\"赋值给变量").append(idName);
                error(err,node->getLine());
            }
        }
        else if(element->getKind()=="bool"){
            if(value=="true"){
                table.getAllLevel(idName,level)->setStringVal("true");
            }
            else if(value=="false"){
                table.getAllLevel(idName,level)->setStringVal("false");
            }
            else{
                string err = "不能把\"";
                err.append(value).append("\"赋值给变量").append(idName);
                error(err,node->getLine());
            }
        }
        else if(element->getKind()=="string"){
            table.getAllLevel(idName,level);
        }
    }
    else{
        string err = "变量";
        err.append(idName).append("在使用前未声明");
        error(err,node->getLine());
    }
}

void Semantic::writeStmt(TreeNode *node)
{
    QString ws;
    string content = node->getContent();
    string kind = node->getNodekind();
    //current use cout
    if(kind=="整数"||kind=="实数"||kind=="字符串"){
        cout<<content<<endl;
    }
    else if(kind==TreeNodeType::IdentifierK){
        if(checkID(node,level)){
            if(node->childNum()!=0){
                string s = array(node->getChild(0),table.getAllLevel(content,level)->getArrayEleCnt());
                if(s!=NULL_STRING)
                    content.append("@").append(s);
                else return;
            }
            TableNode* temp = table.getAllLevel(content,level);
            if(temp->getKind()=="int"){
                this->w->getTextEdit()->append(ws.fromStdString(temp->getIntVal()).append("\n"));
                cout<<temp->getIntVal()<<endl;
            }
            else if(temp->getKind()=="real"){
                this->w->getTextEdit()->append(ws.fromStdString(temp->getRealVal()).append("\n"));
                cout<<temp->getRealVal()<<endl;
            }
            else
                cout<<temp->getStringVal()<<endl;
        }
        else return;
    }
    else if(content==TokenType::PLUS||content==TokenType::MINUS
            ||content==TokenType::MULTI||content==TokenType::DIVIDE){
        string value = expression(node);
        if(value!=NULL_STRING){
            this->w->getTextEdit()->append(ws.fromStdString(value).append("\n"));
            cout<<value<<endl;
        }
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
                        string s = array(node->getChild(i)->getChild(0),tn->getArrayEleCnt());
                        if(s != NULL_STRING)
                            tempContent.append("@").append(s);
                        else return false;
                    }
                    TableNode* tmp = table.getAllLevel(tempContent,level);
                    if(tmp->getKind()=="int")
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
            const double eps = 1e-6;
            if(content==TokenType::GT ){
                if(ele1 > ele2)
                    return true;
            }
            else if(content==TokenType::LT){
                if(ele1 < ele2)
                    return true;
            }
            else if(content==TokenType::EQUAL){
                if(fabs(ele1-ele2)<eps)
                    return true;
            }
            else{
                if(fabs(ele1-ele2)>=eps){
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
    bool isInt = true;
    string content = node->getContent();
    string results[2];
    for(int i = 0; i<node->childNum(); i++){
        TreeNode* tempNode = node->getChild(i);
        string kind = tempNode->getNodekind();
        string tempContent = tempNode->getContent();
        if(kind=="整数"){
            results[i] = tempContent;
        }
        else if(kind=="实数"){
            results[i] = tempContent;
            isInt = false;
        }
        else if(kind==TreeNodeType::IdentifierK){
            if(checkID(tempNode,level)){
                if (tempNode->childNum() != 0) {
                string s = array(tempNode->getChild(0), table
                        .getAllLevel(tempContent, level)
                        ->getArrayEleCnt());
                if (s != NULL_STRING)
                    tempContent += "@" + s;
                else
                    return NULL_STRING;
                }
                TableNode* temp = table.getAllLevel(tempNode
                        ->getContent(), level);
                if (temp->getKind()=="int") {
                    results[i] = temp->getIntVal();
                } else if (temp->getKind()=="real") {
                    results[i] = temp->getRealVal();
                    isInt = false;
                }
            }
            else return NULL_STRING;
        }
        else if(tempContent==TokenType::PLUS||tempContent==TokenType::MINUS||
                tempContent==TokenType::MULTI||tempContent==TokenType::DIVIDE){
                string result = expression(node->getChild(i));
                if(result!=NULL_STRING){
                    results[i]= result;
                    if(matchReal(result))
                        isInt = false;
                }
                else return NULL_STRING;
        }

    }
    //计算
    if(isInt){
        int i1 = atoi(results[0].c_str());
        int i2 = atoi(results[1].c_str());
        if(content==TokenType::PLUS)
            return to_string(i1+i2);
        else if(content==TokenType::MINUS)
            return to_string(i1-i2);
        else if(content==TokenType::MULTI)
            return to_string(i1*i2);
        else
            return to_string(i1/i2);

    }
    else{
        double i1 = atof(results[0].c_str());
        double i2 = atof(results[1].c_str());
        if(content==TokenType::PLUS)
            return to_string(i1+i2);
        else if(content==TokenType::MINUS)
            return to_string(i1-i2);
        else if(content==TokenType::MULTI)
            return to_string(i1*i2);
        else
            return to_string(i1/i2);
    }
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
            if(temp->getKind()=="int"){
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

bool Semantic::checkID(TreeNode *node, int level)
{
    string idname = node->getContent();
    if(table.getAllLevel(idname,level)==nullptr){
        string err = "未定义的标识符";
        error(err,node->getLine());
        return false;
    }
    else{
        if(node->childNum()!=0){
            string tempstr = array(node->getChild(0),table.getAllLevel(idname,level)->getArrayEleCnt());
            if(tempstr!=NULL_STRING){
                idname.append("@").append(tempstr);
            }
            else{
                return false;
            }
        }
        TableNode *element = table.getAllLevel(idname,level);
        if(element->getIntVal()=="" && element->getRealVal()==""  && element->getStringVal()==""){
            string err = "变量";
            err.append(idname).append("在使用前未初始化");
            error(err,node->getLine());
            return false;
        }
        else return true;
    }
}

string Semantic::readInput()
{
    //将读取的输入作为字符串
//    QMutex qmutex;
//    qmutex.lock();
//    auto textEdit = this->w->getTextEdit();
//    textEdit->setText("Please Input:\n");
//    compile::semaphore.wait(&qmutex);
//    qmutex.unlock();
    //okBtn->clicked() = []{}
    this->pause();
    while(!this->flag){
        sleep(1);
    }

    if(userInput==""){
        throw runtime_error("Input is NULL");
    }
    return userInput;
}
