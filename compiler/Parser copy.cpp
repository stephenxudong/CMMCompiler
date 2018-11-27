//
//  Parser.cpp
//  CMM
//
//  Created by Stephen Liao on 2018/10/9.
//  Copyright © 2018 Stephen. All rights reserved.
//

#include "Parser.h"
#include "../type/tokentype.h"
#include <stdarg.h>
using namespace TreeNodeType;
TreeNode *Parser::parse() {
    for(;hasNextToken();){
        root->child.push_back(parseStmt());
    }
    return root;
}

/**
 statement -> if_stm | while_stm | read_stm | write_stm | assign_stm |
 * declare_stm | stmt-block;
 @return syntax tree node
 */
TreeNode *Parser::parseStmt() {
    TreeNode* tmpNode = nullptr;
    //string type = getNextTokenType();
    string type = this->curr.getType();
    if(type=="OVER")return nullptr;
    if(!type.compare(TokenType::IF))
        tmpNode = parseIfStmt();
    else if(type==TokenType::FOR)
        tmpNode = parseForStmt();
    else if(!type.compare(TokenType::WHILE))
        tmpNode = parseWhileStmt();
    else if(!type.compare(TokenType::READ))
        tmpNode = parseReadStmt();
    else if(!type.compare(TokenType::WRITE))
        tmpNode = parseWriteStmt();
    else if(!type.compare(TokenType::LBRACE))
        tmpNode = parseStmtBlock();
    else if(!type.compare(TokenType::IDENTIFIER))
        tmpNode = parseAssignStmt();
    else if(!type.compare(TokenType::INT)||!type.compare(TokenType::REAL))
        tmpNode = parseDeclStmt();
    else{
        string err = "语句以错误的token开始";
        error(TreeNodeType::ErrorK);
       // cout<<err<<endl;
        
    }
    return tmpNode;
}

TreeNode *Parser::parseForStmt()
{
    bool hasBrace = true;
    TreeNode* forNode = new TreeNode("Keyword",TreeNodeType::ForK);
    nextToken();
    //Left Paren
    if(curr.getType()==TokenType::LPAREN){
        nextToken();
    }else{
        string error = "for 循环缺少左括号\"(\" ";
        this->error(error);
        forNode->child.push_back(new TreeNode(TreeNodeType::ErrorK));
    }
    //initialization
    TreeNode* initNode = new TreeNode("initialization","initialization",this->curr.getLine());
    initNode->addChild(parseAssignStmt(true));
    forNode->addChild(initNode);
    //simicolon
    if(curr.getType()==TokenType::SEMICOLON){
        nextToken();
    }else{
        string err = "此处应该是分号";
        error(err);
        delete forNode;//善后
        return new TreeNode(TreeNodeType::ErrorK,err);
    }
    // condition
    TreeNode* condiNode = new TreeNode(TreeNodeType::ConditionK,"Condition",this->curr.getLine());
    condiNode->addChild(parseConditionExp());
    forNode->addChild(condiNode);

    //simicolon
    if(curr.getType()==TokenType::SEMICOLON){
        nextToken();
    }else{
        string err = "此处应该是分号";
        error(err);
        return new TreeNode(TreeNodeType::ErrorK,err);
    }
    //change
    TreeNode* changeNode = new TreeNode("Change","Change",this->curr.getLine());
    changeNode->addChild(parseAssignStmt());
    forNode->addChild(changeNode);
    //Right Paren
    if(curr.getType()==TokenType::RPAREN){
        nextToken();
    }else{
        string error = "for 循环缺少右括号\")\" ";
        this->error(error);
        forNode->child.push_back(new TreeNode(TreeNodeType::ErrorK));
    }
    if(curr.getType()==TokenType::LBRACE){
        nextToken();
    }else{
        hasBrace = false;
    }

    TreeNode* stmtNode = new TreeNode(TreeNodeType::Stmtk,"Statements",curr.getLine());
    if(hasBrace){
        while(hasNextToken()){
            if(curr.getType()!=TokenType::RBRACE)
                stmtNode->addChild(parseStmt());
            else if(stmtNode->childNum()==0){
                forNode->remove(forNode->childNum()-1);
                stmtNode->setContent("EmptyStmt");
                forNode->addChild(stmtNode);
                break;
            }
            else{
                break;
            }
        }
        if(curr.getType()==TokenType::RPAREN){
            nextToken();
        }else{
            string error = "for 循环缺少右括号\")\" ";
            this->error(error);
            forNode->child.push_back(new TreeNode(TreeNodeType::ErrorK));
        }

    }else{
        stmtNode->addChild(parseStmt());
    }

    return forNode;
}

TreeNode *Parser::parseExp() {
    TreeNode* exp = term();
    //下一个token是+/-
    while(curr.getType()==TokenType::PLUS
          ||curr.getType()==TokenType::MINUS){
        //add_op
        TreeNode* addNode = add_op();
        addNode->addChild(exp);
        exp = addNode;
        exp->addChild(term());
    }
    return exp;
}

TreeNode *Parser::add_op()
{
    TreeNode* tmpNode = nullptr;
    if(curr.getType()==TokenType::PLUS){
        tmpNode = new TreeNode(TreeNodeType::operatorK,TokenType::PLUS,curr.getLine());
        nextToken();
    }else if(curr.getType()==TokenType::MINUS){
        tmpNode = new TreeNode(TreeNodeType::operatorK,TokenType::MINUS,curr.getLine());
        nextToken();
    }else{
        string err = "加减符号出错";
        error(err);
        return new TreeNode(TreeNodeType::ErrorK,err);
    }
    return tmpNode;
}


TreeNode *Parser::parseStmtBlock() {
    TreeNode* node = new TreeNode(StmtK);
    TreeNode* header = node;
    TreeNode* temp = nullptr;
    consumeNextToken(TokenType::LBRACE);
    while (this->hasNextToken()) {
        if(getNextTokenType() == TokenType::RBRACE )
            break;
        //允许语句块中没有语句
        temp = parseStmt();
        node->child.push_back(temp);
        node = temp;
    }
    if(!hasNextToken()){
        if(getNextTokenType()==TokenType::RBRACE)
        // find RBrace unitl the end of file, then error
        errnum++;
        string errmsg;
        nextToken();
        errmsg.append("    ERROR:第 ")
        .append(to_string(curr.getLine()))
        .append(" 行,第 ")
        .append(to_string(curr.getPosition()))
        .append(" 列：")
        .append("缺少语句块的结束}标志\n");
        this->errors.push_back(errmsg);
        cout<<errmsg<<endl;
    }
    else consumeNextToken(TokenType::RBRACE);
    return header;
}


/**
 if_stmt -> IF LPAREN condition-exp RPAREN LBRACE? statement RBRACE? (ELSE LBRACE
 * statement RBRACE)?;

 @return if-stmt
 */
TreeNode *Parser::parseIfStmt() {
    bool hasIfBrace = true;
    bool hasElseBrace = true;
    TreeNode* ifnode = new TreeNode(TreeNodeType::Ifk,"If",curr.getLine());
    nextToken();
    //Left Paren
    checkCurrentType(TokenType::LPAREN,ifnode);
    //condition
    TreeNode* condiNode = new TreeNode(TreeNodeType::ConditionK,"Condition",this->curr.getLine());
    condiNode->addChild(parseConditionExp());
    ifnode->addChild(condiNode);
    //right Paren
    checkCurrentType(TokenType::RPAREN,ifnode);

    //left brace
    if(curr.getType()==TokenType::LBRACE){
        nextToken();
    }else{
        hasIfBrace = false;
    }
    TreeNode* stmtNode = new TreeNode(TreeNodeType::Stmtk,"Statement",curr.getLine());
    ifnode->addChild(stmtNode);
    if(hasIfBrace){
        while(hasNextToken()){
            if(curr.getType()!=TokenType::RBRACE)
                stmtNode->addChild(parseStmt());
            else if(stmtNode->childNum()==0){
                ifnode->remove(ifnode->childNum()-1);
                stmtNode->setContent("EmptyStmt");
                ifnode->addChild(stmtNode);
                break;
            }
            else{
                break;
            }
        }
        checkCurrentType(TokenType::RBRACE,ifnode);

    }else{
        if(hasNextToken())
            stmtNode->addChild(parseStmt());
    }
    // else part
    if(curr.getType()==TokenType::ELSE){
        TreeNode* elseNode = new TreeNode(TreeNodeType::ElseK,"Else",curr.getLine());
        ifnode->addChild(elseNode);
        nextToken();
        // has left brace ?
        if(curr.getType()==TokenType::LBRACE){
            nextToken();
        }else{
            hasElseBrace = false;
        }
        if(hasElseBrace){
            while(hasNextToken()){
                if(curr.getType()!=TokenType::RBRACE)
                    stmtNode->addChild(parseStmt());
            }
            checkCurrentType(TokenType::RBRACE,ifnode);

        }else{
            if(hasNextToken())
                stmtNode->addChild(parseStmt());
        }

    }
    return ifnode;
    // if-block 和 else-block 可能有 大括号
//    consumeNextToken(TokenType::IF);
//    consumeNextToken(TokenType::LPAREN);
//    //处理括号中的表达式
//    ifnode->child.push_back(parseConditionExp());
//    consumeNextToken(TokenType::RPAREN);
//    //单一语句或者语句块
//    ifnode->child.push_back(parseStmt());
////    bool hasIfBrace = true;
////    if(getNextTokenType()==tokenType::LBRACE){
////        nextToken();
////    }else{ hasIfBrace = false; }
////    //有大括号
////    if(hasIfBrace){
////        while(tokenHasNext()){

////        }
////    }
//    nextToken();
//    if(!curr.getType().compare(TokenType::ELSE)){
//        TreeNode* elsenode= new TreeNode(ElseK);
//        elsenode->child.push_back(parseStmt());
//        ifnode->child.push_back(elsenode);
//    }

}


/**
 while-stmt -> while ( exp ) stmt-block

 @return while-node
 */
TreeNode *Parser::parseWhileStmt() {
    bool hasBrace = true;
    TreeNode* whileNode = new TreeNode(TreeNodeType::WhileK,"While",curr.getLine());
    nextToken();
    //Left Paren
    checkCurrentType(TokenType::LPAREN,whileNode);
    //condition
    TreeNode* condiNode = new TreeNode(TreeNodeType::ConditionK,"Condition",this->curr.getLine());
    condiNode->addChild(parseConditionExp());
    whileNode->addChild(condiNode);
    //right Paren
    checkCurrentType(TokenType::RPAREN,whileNode);

    //left brace
    if(curr.getType()==TokenType::LBRACE){
        nextToken();
    }else{
        hasBrace = false;
    }
    TreeNode* stmtNode = new TreeNode(TreeNodeType::Stmtk,"Statement",curr.getLine());
    whileNode->addChild(stmtNode);
    if(hasBrace){
        while(hasNextToken()){
            if(curr.getType()!=TokenType::RBRACE)
                stmtNode->addChild(parseStmt());
            else if(stmtNode->childNum()==0){
                whileNode->remove(whileNode->childNum()-1);
                stmtNode->setContent("EmptyStmt");
                whileNode->addChild(stmtNode);
                break;
            }
            else{
                break;
            }
        }
        checkCurrentType(TokenType::RBRACE,whileNode);

    }else{
        if(hasNextToken())
            stmtNode->addChild(parseStmt());
    }
    return whileNode;
//    consumeNextToken(TokenType::WHILE);
//    consumeNextToken(TokenType::LPAREN);
//    whileNode->child.push_back(parseConditionExp());
//    consumeNextToken(TokenType::RPAREN);
//    auto type = getNextTokenType();
//    whileNode->child.push_back(parseStmt());
//    if(type!=TokenType::LBRACE && whileNode->child.size()>1 ){
//        errnum++;
//        string errmsg;
//        nextToken();
//        errmsg.append("    ERROR:第 ")
//        .append(to_string(curr.getLine()))
//        .append(" 行,第 ")
//        .append(to_string(curr.getPosition()))
//        .append(" 列：")
//        .append("while语句缺少语句块开始的{\n");
//        this->errors.push_back(errmsg);
//        cout<<errmsg<<endl;
//    }
    
//    return whileNode;
}




/**
 assignStmt -> variable addtive-exp ;

 @return assignment-node
 */
TreeNode *Parser::parseAssignStmt(bool inFor) {
    //返回的根节点是assign运算符
    TreeNode* node = new TreeNode(TreeNodeType::operatorK,TokenType::ASSIGN,curr.getLine());
    TreeNode* idNode = new TreeNode(TreeNodeType::IdentifierK,curr.getText(),curr.getLine());
    node->addChild(idNode);
    nextToken();
    if(curr.getType()==TokenType::LBRACKET)
        idNode->addChild(array());
    //匹配=
    if(curr.getType()==TokenType::ASSIGN){
        nextToken();
    }else{
        string err = "赋值语句缺少\"=\" ";
        error(err);
        node->releaseChild();
        delete node;
        return new TreeNode(TreeNodeType::ErrorK + err);
    }
    node->addChild(parseConditionExp());
    if(!inFor){
        //不是在for中调用，需要匹配分号
        checkCurrentType(TokenType::SEMICOLON);
    }

//    node->child.push_back(variable());
//    consumeNextToken(TokenType::ASSIGN);
//    node->child.push_back(parseAddtiveExp());
//    consumeNextToken(TokenType::SEMICOLON);
    return node;
}



/**
 declStmt -> int|real identifier = exp

 @return decl-stmt
 */
TreeNode *Parser::parseDeclStmt() {
    TreeNode* node = new TreeNode(TreeNodeType::declK,curr.getText(),curr.getLine());
    nextToken();
    node = this->delc_aid(node);
    string next = "";
    while(hasNextToken()){
        next = curr.getType();
        if(next==TokenType::COMMA){
            nextToken();
            node = this->delc_aid(node);
        }else{
            break;
        }
        if(hasNextToken()){
            next = curr.getText();
        }
    }
    //semicolon
    checkCurrentType(TokenType::SEMICOLON,node);
    return node;
//    TreeNode* varNode = new TreeNode(VarK);
//    node->child.push_back(varNode);
//    if(checkNextTokenType({TokenType::REAL,TokenType::INT})){
//        nextToken();
//        // 此时token是real or int
//        auto type = curr.getType();
//        varNode->type = type;// int or real
//    }else{
//        errnum++;
//        string errmsg;
//        errmsg.append("    ERROR:第 ")
//        .append(to_string(curr.getLine()))
//        .append(" 行,第 ")
//        .append(to_string(curr.getPosition()))
//        .append(" 列：")
//        .append("此处应当是: ")
//        .append("int 或者 real ").append("类型\n");
//        this->errors.push_back(errmsg);
//    }
//    if(getNextTokenType()==TokenType::IDENTIFIER){
//        nextToken();
//        // 此时当前token是identifier
//        varNode->val = curr.getText();
//    }else{
//        errnum++;
//        string errmsg;
//        errmsg.append("    ERROR:第 ")
//        .append(to_string(curr.getLine()))
//        .append(" 行,第 ")
//        .append(to_string(curr.getPosition()))
//        .append(" 列：")
//        .append("此处应当是: ")
//        .append("标识符 ").append("类型\n");
//        this->errors.push_back(errmsg);
//    }
//    if(getNextTokenType()==TokenType::ASSIGN){
//        consumeNextToken(TokenType::ASSIGN);
//        if(getNextTokenType()==TokenType::LPAREN){
//            consumeNextToken(TokenType::LPAREN);
//            node->child.push_back(parseAddtiveExp());
//            consumeNextToken(TokenType::RPAREN);
//        }
//        else node->child.push_back(parseAddtiveExp());
//    }else if(getNextTokenType()==TokenType::LBRACKET){
//        // 数组类型
//        consumeNextToken(TokenType::LBRACKET);
//        node->child.push_back(parseAddtiveExp());
//        consumeNextToken(TokenType::RBRACKET);
//    }
//    consumeNextToken(TokenType::SEMICOLON);
//    return node;
}

TreeNode *Parser::delc_aid(TreeNode* &node)
{
    if(curr.getType()==TokenType::IDENTIFIER){
        TreeNode* idNode = new TreeNode(TreeNodeType::IdentifierK,curr.getText(),curr.getLine());
        node->addChild(idNode);
        nextToken();
        if(curr.getType()==TokenType::LBRACKET){
            idNode->addChild(array());
        }
        else if(curr.getType()==TokenType::ASSIGN||
                curr.getType()==TokenType::SEMICOLON||
                curr.getText()==TokenType::COMMA){
            string err = "声明语句出错，标识符后面出现不正确的token";
            error(err);
            node->addChild(new TreeNode(TreeNodeType::ErrorK,err));
            nextToken();

        }
    }else{
        string err = "声明语句中标识符出错";
        error(err);
        node->addChild(new TreeNode(TreeNodeType::ErrorK,err));
        nextToken();
    }
    if(curr.getType()==TokenType::ASSIGN){
        TreeNode* assignNode = new TreeNode("分隔符",TokenType::ASSIGN,curr.getLine());
        node->addChild(assignNode);
        nextToken();
        assignNode->addChild(parseConditionExp());
    }
    return node;
}


/**
 read-stmt -> read(variable) ;

 @return read-node
 */
TreeNode *Parser::parseReadStmt() {
    TreeNode* node = nullptr;
    nextToken();
    //left paren
    if(curr.getType()==TokenType::LPAREN){
        nextToken();
    }else{
        string err = "Read语句缺少左括号";
        error(err);
        //此处不需要善后
        return new TreeNode(TreeNodeType::ErrorK,err);

    }
    if(curr.getType()==TokenType::IDENTIFIER){
        node = new TreeNode(TreeNodeType::IdentifierK,curr.getText(),curr.getLine());
        nextToken();
        if(curr.getType()==TokenType::LBRACKET)
            node->addChild(array());
    }else{
        string err = "Read语句中此处应该是标识符";
        error(err);
        nextToken();
        return new TreeNode(TreeNodeType::ErrorK,err);
    }

    //right paren
    if(curr.getType()==TokenType::RPAREN){
        nextToken();
    }else{
        string err = "Read语句缺少右括号";
        error(err);
        delete node;
        return new TreeNode(TreeNodeType::ErrorK,err);

    }

    //simicolon
    if(curr.getType()==TokenType::RPAREN){
        nextToken();
    }else{
        string err = "Read语句缺少分号";
        error(err);
        delete node;
        return new TreeNode(TreeNodeType::ErrorK,err);

    }
//    consumeNextToken(TokenType::READ);
//    if(!consumeNextToken(TokenType::LPAREN)){
//        // 缺少左括号
//        nextToken();
//    }
//    readNode->child.push_back(variable());
//    consumeNextToken(TokenType::RPAREN);//此时并不需要nexttoken
//    if(!consumeNextToken(TokenType::SEMICOLON)){
//        nextToken();
//    }
    return node;
}


/**
 write-stmt -> write(exp) ;

 @return write-node
 */
TreeNode *Parser::parseWriteStmt() {
    TreeNode* node = nullptr;
    nextToken();
    //left paren
    if(curr.getType()==TokenType::LPAREN){
        nextToken();
    }else{
        string err = "write语句缺少左括号";
        error(err);
        //此处不需要善后
        return new TreeNode(TreeNodeType::ErrorK,err);

    }
    node = parseExp();

    //right paren
    if(curr.getType()==TokenType::RPAREN){
        nextToken();
    }else{
        string err = "Read语句缺少右括号";
        error(err);
        delete node;
        return new TreeNode(TreeNodeType::ErrorK,err);

    }

    //simicolon
    if(curr.getType()==TokenType::RPAREN){
        nextToken();
    }else{
        string err = "Read语句缺少分号";
        error(err);
        delete node;
        return new TreeNode(TreeNodeType::ErrorK,err);

    }
//    TreeNode* node = new TreeNode(WriteK);
//    consumeNextToken(TokenType::WRITE);
//    consumeNextToken(TokenType::LPAREN);
//    node->child.push_back(parseAddtiveExp());
//    consumeNextToken(TokenType::RPAREN);
//    consumeNextToken(TokenType::SEMICOLON);
    return node;
}



/**
 conditionExp -> addtiveExp cond-op addtive-exp | addtive-exp
 
 @return TreeNode
 */
TreeNode *Parser::parseConditionExp() {
    //记录expreesion生成的结点
    TreeNode* tmpNode = parseExp();
    if(curr.getType()==TokenType::EQUAL
            ||curr.getType()==TokenType::NEQUAL
            || curr.getType()==TokenType::GT
            || curr.getType()==TokenType::LT ){
        TreeNode* comNode = comparison_op();
        comNode->addChild(tmpNode);
        comNode->addChild(parseExp());
        return comNode;
    }
    return tmpNode;

//    TreeNode* exp = new TreeNode(ConditionK);
//    auto leftExp = parseAddtiveExp();
//    vector<string> vec = {TokenType::GT, TokenType::LT,TokenType::GET,TokenType::LET,TokenType::EQUAL};
//    if(checkNextTokenType(vec)){
//        exp->child.push_back(leftExp);
//        nextToken();
//        exp->child.push_back(conditionSymbol());
//        exp->child.push_back(parseAddtiveExp());
//        return exp;
//    }
//    delete exp;
//    return leftExp;
}

TreeNode *Parser::comparison_op()
{
    TreeNode* tmpNode = nullptr;
    if(curr.getType()==TokenType::LT){
        tmpNode = new TreeNode(TreeNodeType::operatorK,TokenType::LT,curr.getLine());
        nextToken();
    }
    else if(curr.getType()==TokenType::GT){
        tmpNode = new TreeNode(TreeNodeType::operatorK,TokenType::GT,curr.getLine());
        nextToken();
    }
    else if(curr.getType()==TokenType::EQUAL){
        tmpNode = new TreeNode(TreeNodeType::operatorK,TokenType::EQUAL,curr.getLine());
        nextToken();
    }
    else if(curr.getType()==TokenType::NEQUAL){
        tmpNode = new TreeNode(TreeNodeType::operatorK,TokenType::NEQUAL,curr.getLine());
        nextToken();
    }
    else{
        string err = "比较运算符出错";
        error(err);
        return new TreeNode(TreeNodeType::ErrorK,err);
    }
    return tmpNode;
}
/**
 addtive-exp -> term add-op additive-exp | term
 
 @return treeNode
 */
TreeNode *Parser::parseAddtiveExp() {
    TreeNode* addExp = new TreeNode(ExpK);
    auto leftnode = term();
    vector<string> vec = {TokenType::PLUS, TokenType::MINUS};
    if(checkNextTokenType(vec)){
        addExp->child.push_back(leftnode);
        nextToken();
        // 判断是加号还是减号
        if(curr.getType()==TokenType::PLUS)
            addExp->child.push_back(new TreeNode(plusOp));
        else addExp->child.push_back(new TreeNode(minusOp));
        //递归处理后半段表达式
        addExp->child.push_back(parseAddtiveExp());
        return addExp;
    }
    delete addExp;
    return leftnode;
}


/**
 term -> factor mul-op term | factor

 @return term-treeNode
 */
TreeNode *Parser::term() {
    TreeNode* tmpNode = factor();
    while(curr.getType()==TokenType::MULTI
          ||curr.getType()==TokenType::DIVIDE){
        TreeNode* mulNode = mul_op();
        mulNode->addChild(tmpNode);
        tmpNode = mulNode;
        tmpNode->addChild(factor());

    }
    return tmpNode;
//    if(checkNextTokenType(vec)){
//        termNode->child.push_back(leftNode);
//        nextToken();
//        // 判断是加号还是减号
//        if(curr.getType()==TokenType::MULTI)
//            termNode->child.push_back(new TreeNode(multiOp));
//        else termNode->child.push_back(new TreeNode(divideOp));
//        nextToken();
//        //递归处理后半段表达式
//        termNode->child.push_back(term());
//        return termNode;
//    }
//    delete termNode;
    //    return leftNode;
}

TreeNode *Parser::mul_op()
{
    TreeNode* tmpNode = nullptr;
    if(curr.getType()==TokenType::MULTI){
        tmpNode = new TreeNode(TreeNodeType::operatorK,TokenType::MULTI,curr.getLine());
        nextToken();
    }else if(curr.getType()==TokenType::DIVIDE){
        tmpNode = new TreeNode(TreeNodeType::operatorK,TokenType::DIVIDE,curr.getLine());
        nextToken();
    }else{
        string err = "乘除符号出错";
        error(err);
        return new TreeNode(TreeNodeType::ErrorK,err);
    }
    return tmpNode;
}


/**
 factor -> ( exp ) | number | variable | Add-opexp

 @return factor-node
 */
TreeNode *Parser::factor() {
    TreeNode* tmpNode = nullptr;
    if(curr.getType()==TokenType::LITERAL_INT){
        tmpNode = new TreeNode("整数",curr.getText(),curr.getLine());
        nextToken();
    }
    else if(curr.getType()==TokenType::LITERAL_REAL){
        tmpNode = new TreeNode("实数",curr.getText(),curr.getLine());
        nextToken();
    }
    else if(curr.getType()==TokenType::LITERAL_TRUE){
        tmpNode = new TreeNode("布尔值",curr.getText(),curr.getLine());
        nextToken();
    }
    else if(curr.getType()==TokenType::LITERAL_FALSE){
        tmpNode = new TreeNode("布尔值",curr.getText(),curr.getLine());
        nextToken();
    }
    else if(curr.getType()==TokenType::IDENTIFIER){
        tmpNode = new TreeNode(TreeNodeType::IdentifierK,curr.getText(),curr.getLine());
        nextToken();
        if(curr.getType()==TokenType::LBRACKET){
            tmpNode->addChild(array());
        }
    }
    else if(curr.getType()==TokenType::LPAREN){
        nextToken();
        tmpNode = parseExp();
        if(curr.getType()==TokenType::RPAREN){
            nextToken();
        }else{
            string err = "算式因子缺少右括号";
            error(err);
            tmpNode->releaseChild();
            delete tmpNode;
            return new TreeNode(TreeNodeType::ErrorK,err);
        }
    }
    else if(curr.getType()==TokenType::LITERAL_STRING){
        tmpNode = new TreeNode("字符串",curr.getText(),curr.getLine());
        nextToken();
    }
    else{
        string err = " 算式因子存在错误";
        error(err);
        //匹配分号
        if(curr.getType()==TokenType::SEMICOLON)
            nextToken();
        return new TreeNode(TreeNodeType::ErrorK,err);
    }
    return tmpNode;

//    TreeNode* node = new TreeNode(FactorK);
//    auto t = getNextTokenType();
//    if(t==TokenType::LITERAL_INT|| t==TokenType::LITERAL_REAL){
//        TreeNode* literalNode = literal();
//        node->child.push_back(literalNode);
//    }
//    else if(t==TokenType::LPAREN){
//        consumeNextToken(TokenType::LPAREN);
//        delete node;
//        node = parseAddtiveExp();
//        consumeNextToken(TokenType::RPAREN);
//    }
//    else if(t==TokenType::IDENTIFIER){
////        TreeNode* identiNode = new TreeNode(ExpK,);
//        node->child.push_back(this->variable());
//    }
//    return node;
}



/**
 ConditionOperator -> plusOp | minusOp | multiOp | divideOp

 @return cnditionSymbol-TreeNode
 */
TreeNode *Parser::conditionSymbol() {
    TreeNode* optor = NULL;
    string type = curr.getType();
    if(!type.compare(TokenType::GT) || !type.compare(TokenType::LT)||
       !type.compare(TokenType::GET) || !type.compare(TokenType::LET)||
       !type.compare(TokenType::EQUAL)){
        optor = new TreeNode(ConditionK);
        optor->type = type;
    }
    return optor;
}

bool Parser::consumeNextToken(std::string type) {
    if(getNextTokenType()==type){
        nextToken();
        return true;
    }
    else{
        errnum++;
        string errmsg;
        errmsg.append("    ERROR:第 ")
        .append(to_string(curr.getLine()))
        .append(" 行,第 ")
        .append(to_string(curr.getPosition()))
        .append(" 列：")
        .append("此处应当是: ")
        .append(type).append("类型\n");
        this->errors.push_back(errmsg);
        cout<<errmsg;
        return false;
    }
}

void Parser::error(string error)
{
    string line = " ERROR:第 ";
    Token previous = tokens[this->index-1];
    if(this->curr.getLine()==previous.getLine()){
        line.append(to_string(this->curr.getLine()))
            .append(" 行，第 ")
            .append(to_string(this->curr.getPosition()))
            .append(" 列");
    }else{
        line.append(to_string(previous.getLine()))
            .append(" 行，第 ")
            .append(to_string(previous.getPosition()))
            .append(" 列: ");
    }
    line.append(error);
    this->errors.push_back(line);
}

void Parser::checkCurrentType(const string &type, TreeNode *node)
{
    if(curr.getType()==type){
        nextToken();
    }else{
        string error = "此处缺少 ";
        error.append(type);
        this->error(error);
        node->child.push_back(new TreeNode(TreeNodeType::ErrorK));
    }
}


bool Parser::checkNextTokenType(int n, ...) {
    auto type = this->getNextTokenType();
    va_list args;
    va_start(args, n);//初始化args指向强制参数arg的下一个参数
    while(n > 0){
        //通过va_arg(args, char*)依次获取参数的值
//#ifdef DEBUG
//        auto s =va_arg(args, char*);
//        std::cout<<s<<" ";
//#endif
        if(!type.compare(va_arg(args, char*)))
            return true;
        n--;
    }
    va_end(args);//释放args
    return false;
}

/**
 检查接下来一个token的类型是否在vector之内,因为想要使用列表来初始化vector，所以采用传值的方式

 @param types vector<string>
 @return bool
 */
bool Parser::checkNextTokenType(vector<string> types) {
    auto type = this->getNextTokenType();
    for(auto i : types){
        if(!type.compare(i))
            return true;
    }
    return false;
}


/**
 literal -> int_val | read_val

 @return literal_node
 */
TreeNode *Parser::literal() {
    nextToken();
    TreeNode* node = new TreeNode(LiteralK);

    node->type = curr.getType();
    node->val = curr.getText();
    return node;
}



/**
 variable -> identifier | identifier[index]

 @return variable-node
 */
TreeNode *Parser::variable() {
    TreeNode* node = new TreeNode(VarK);
    if(checkNextTokenType(1,TokenType::IDENTIFIER.c_str())){
        nextToken();
        node->type = curr.getType();
        node->val = curr.getText();
    }else{
        errnum++;
        string errmsg;
        errmsg.append("    ERROR:第 ")
        .append(to_string(curr.getLine()))
        .append(" 行,第 ")
        .append(to_string(curr.getPosition()))
        .append(" 列：")
        .append("此处应当是: ")
        .append("id").append("类型\n");
        this->errors.push_back(errmsg);
    }
    if(checkNextTokenType(1,TokenType::LBRACKET.c_str())){
        consumeNextToken(TokenType::LBRACKET);
        node->child.push_back(parseAddtiveExp());
        consumeNextToken(TokenType::RBRACKET);
    }

    return node;
}

TreeNode *Parser::array()
{
    TreeNode* tmpNode = nullptr;
    if(curr.getType()==TokenType::LBRACKET)
        nextToken();
    else{
        string err = "缺少左中括号";
        error(err);
        return new TreeNode(TreeNodeType::ErrorK,err);

    }
    tmpNode = parseExp();
    if(curr.getType()==TokenType::RBRACKET)
        nextToken();
    else{
        string err = "缺少做右中括号";
        error(err);
        return new TreeNode(TreeNodeType::ErrorK,err);
    }
    return tmpNode;
}

void Parser::printT(TreeNode *t,char* outpath) {
    ostream& out = cout;
    printTree(t,0);
}

void Parser::printTree(TreeNode *root, int depth) {
    for(int d = depth; d > 0; d--)
        cout<<"\t";
    cout<<root->nodekind<<endl;
    int i;
    for(i = 0; i < root->child.size(); i++){
        printTree(root->child[i],depth+1);
    }
}























