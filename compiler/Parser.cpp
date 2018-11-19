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
    for(;tokenHasNext();){
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
    TreeNode* tmpNode = NULL;
    string type = getNextTokenType();
    if(type=="OVER")return NULL;
    if(!type.compare(tokenType::IF))
        tmpNode = parseIfStmt();
    else if(!type.compare(tokenType::WHILE))
        tmpNode = parseWhileStmt();
    else if(!type.compare(tokenType::READ))
        tmpNode = parseReadStmt();
    else if(!type.compare(tokenType::WRITE))
        tmpNode = parseWriteStmt();
    else if(!type.compare(tokenType::LBRACE))
        tmpNode = parseStmtBlock();
    else if(!type.compare(tokenType::IDENTIFIER))
        tmpNode = parseAssignStmt();
    else if(!type.compare(tokenType::INT)||!type.compare(tokenType::REAL))
        tmpNode = parseDeclStmt();
    else{
        errnum++;
        string errmsg;
        nextToken();
        errmsg.append("    ERROR:第 ")
        .append(to_string(curr.getLine()))
        .append(" 行,第 ")
        .append(to_string(curr.getPosition()))
        .append(" 列：")
        .append(curr.getText()).append("是未知结构的开始\n");
        this->errors.push_back(errmsg);
        cout<<errmsg<<endl;
        
    }
       return tmpNode;
}

TreeNode *Parser::parseExp() {
    TreeNode* exp = new TreeNode(ExpK);
    return exp;
}


TreeNode *Parser::parseStmtBlock() {
    TreeNode* node = new TreeNode(StmtK);
    TreeNode* header = node;
    TreeNode* temp = NULL;
    consumeNextToken(tokenType::LBRACE);
    while (getNextTokenType() != tokenType::RBRACE) {//允许语句块中没有语句
        temp = parseStmt();
        node->child.push_back(temp);
        node = temp;
    }
    consumeNextToken(tokenType::RBRACE);
    return header;
}


/**
 if_stmt -> IF LPAREN condition-exp RPAREN LBRACE? statement RBRACE? (ELSE LBRACE
 * statement RBRACE)?;

 @return if-stmt
 */
TreeNode *Parser::parseIfStmt() {
    TreeNode* ifnode = new TreeNode(IfK);
    // if-block 和 else-block 可能有 大括号
    consumeNextToken(tokenType::IF);
    consumeNextToken(tokenType::LPAREN);
    //处理括号中的表达式
    ifnode->child.push_back(parseConditionExp());
    consumeNextToken(tokenType::RPAREN);
    //单一语句或者语句块
    ifnode->child.push_back(parseStmt());
    nextToken();
    if(!curr.getType().compare(tokenType::ELSE)){
        TreeNode* elsenode= new TreeNode(ElseK);
        elsenode->child.push_back(parseStmt());
        ifnode->child.push_back(elsenode);
    }
    return ifnode;
}


/**
 while-stmt -> while ( exp ) stmt-block

 @return while-node
 */
TreeNode *Parser::parseWhileStmt() {
    TreeNode* tmpNode = new TreeNode(WhileK);
    consumeNextToken(tokenType::WHILE);
    consumeNextToken(tokenType::LPAREN);
    tmpNode->child.push_back(parseConditionExp());
    consumeNextToken(tokenType::RPAREN);
    auto type = getNextTokenType();
    tmpNode->child.push_back(parseStmt());
    if(type!=tokenType::LBRACE && tmpNode->child.size()>1 ){
        errnum++;
        string errmsg;
        nextToken();
        errmsg.append("    ERROR:第 ")
        .append(to_string(curr.getLine()))
        .append(" 行,第 ")
        .append(to_string(curr.getPosition()))
        .append(" 列：")
        .append("while语句缺少语句块开始的{\n");
        this->errors.push_back(errmsg);
        cout<<errmsg<<endl;
    }
    
    return tmpNode;
}




/**
 assignStmt -> variable addtive-exp ;

 @return assignment-node
 */
TreeNode *Parser::parseAssignStmt() {
    TreeNode* node = new TreeNode(AssignK);
    node->child.push_back(variable());
    consumeNextToken(tokenType::ASSIGN);
    node->child.push_back(parseAddtiveExp());
    consumeNextToken(tokenType::SEMICOLON);
    return node;
}



/**
 declStmt -> int|real identifier = exp

 @return decl-stmt
 */
TreeNode *Parser::parseDeclStmt() {
    TreeNode* node = new TreeNode(DeclK);
    TreeNode* varNode = new TreeNode(VarK);
    node->child.push_back(varNode);
    if(checkNextTokenType({tokenType::REAL,tokenType::INT})){
        nextToken();
        // 此时token是real or int
        auto type = curr.getType();
        varNode->type = type;// int or real
    }else{
        errnum++;
        string errmsg;
        errmsg.append("    ERROR:第 ")
        .append(to_string(curr.getLine()))
        .append(" 行,第 ")
        .append(to_string(curr.getPosition()))
        .append(" 列：")
        .append("此处应当是: ")
        .append("int 或者 real ").append("类型\n");
        this->errors.push_back(errmsg);
    }
    if(getNextTokenType()==tokenType::IDENTIFIER){
        nextToken();
        // 此时当前token是identifier
        varNode->val = curr.getText();
    }else{
        errnum++;
        string errmsg;
        errmsg.append("    ERROR:第 ")
        .append(to_string(curr.getLine()))
        .append(" 行,第 ")
        .append(to_string(curr.getPosition()))
        .append(" 列：")
        .append("此处应当是: ")
        .append("标识符 ").append("类型\n");
        this->errors.push_back(errmsg);
    }
    if(getNextTokenType()==tokenType::ASSIGN){
        consumeNextToken(tokenType::ASSIGN);
        if(getNextTokenType()==tokenType::LPAREN){
            consumeNextToken(tokenType::LPAREN);
            node->child.push_back(parseAddtiveExp());
            consumeNextToken(tokenType::RPAREN);
        }
        else node->child.push_back(parseAddtiveExp());
    }else if(getNextTokenType()==tokenType::LBRACKET){
        // 数组类型
        consumeNextToken(tokenType::LBRACKET);
        node->child.push_back(parseAddtiveExp());
        consumeNextToken(tokenType::RBRACKET);
    }
    consumeNextToken(tokenType::SEMICOLON);
    return node;
}


/**
 read-stmt -> read(variable) ;

 @return read-node
 */
TreeNode *Parser::parseReadStmt() {
    TreeNode* readNode = new TreeNode(ReadK);
    consumeNextToken(tokenType::READ);
    if(!consumeNextToken(tokenType::LPAREN)){
        // 缺少左括号
        nextToken();
    }
    readNode->child.push_back(variable());
    consumeNextToken(tokenType::RPAREN);//此时并不需要nexttoken
    if(!consumeNextToken(tokenType::SEMICOLON)){
        nextToken();
    }
    return readNode;
}


/**
 write-stmt -> write(exp) ;

 @return write-node
 */
TreeNode *Parser::parseWriteStmt() {
    TreeNode* node = new TreeNode(WriteK);
    consumeNextToken(tokenType::WRITE);
    consumeNextToken(tokenType::LPAREN);
    node->child.push_back(parseAddtiveExp());
    consumeNextToken(tokenType::RPAREN);
    consumeNextToken(tokenType::SEMICOLON);
    return node;
}



/**
 conditionExp -> addtiveExp cond-op addtive-exp | addtive-exp
 
 @return TreeNode
 */
TreeNode *Parser::parseConditionExp() {
    TreeNode* exp = new TreeNode(ConditionK);
    auto leftExp = parseAddtiveExp();
    vector<string> vec = {tokenType::GT, tokenType::LT,tokenType::GET,tokenType::LET,tokenType::EQUAL};
    if(checkNextTokenType(vec)){
        exp->child.push_back(leftExp);
        nextToken();
        exp->child.push_back(conditionSymbol());
        exp->child.push_back(parseAddtiveExp());
        return exp;
    }
    delete exp;
    return leftExp;
}
/**
 addtive-exp -> term add-op additive-exp | term
 
 @return treeNode
 */
TreeNode *Parser::parseAddtiveExp() {
    TreeNode* addExp = new TreeNode(ExpK);
    auto leftnode = term();
    vector<string> vec = {tokenType::PLUS, tokenType::MINUS};
    if(checkNextTokenType(vec)){
        addExp->child.push_back(leftnode);
        nextToken();
        // 判断是加号还是减号
        if(curr.getType()==tokenType::PLUS)
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
    TreeNode* termNode = new TreeNode(TermK);
    auto leftNode = factor();
    vector<string> vec = {tokenType::MULTI,tokenType::DIVIDE};
    if(checkNextTokenType(vec)){
        termNode->child.push_back(leftNode);
        nextToken();
        // 判断是加号还是减号
        if(curr.getType()==tokenType::MULTI)
            termNode->child.push_back(new TreeNode(multiOp));
        else termNode->child.push_back(new TreeNode(divideOp));
        nextToken();
        //递归处理后半段表达式
        termNode->child.push_back(term());
        return termNode;
    }
    delete termNode;
    return leftNode;
}


/**
 factor -> ( exp ) | number | variable | Add-opexp

 @return factor-node
 */
TreeNode *Parser::factor() {
    TreeNode* node = new TreeNode(FactorK);
    auto t = getNextTokenType();
    if(t==tokenType::LITERAL_INT|| t==tokenType::LITERAL_REAL){
        TreeNode* literalNode = literal();
        node->child.push_back(literalNode);
    }
    else if(t==tokenType::LPAREN){
        consumeNextToken(tokenType::LPAREN);
        delete node;
        node = parseAddtiveExp();
        consumeNextToken(tokenType::RPAREN);
    }
    else if(t==tokenType::IDENTIFIER){
//        TreeNode* identiNode = new TreeNode(ExpK,);
        node->child.push_back(this->variable());
    }
    return node;
}



/**
 ConditionOperator -> plusOp | minusOp | multiOp | divideOp

 @return cnditionSymbol-TreeNode
 */
TreeNode *Parser::conditionSymbol() {
    TreeNode* optor = NULL;
    string type = curr.getType();
    if(!type.compare(tokenType::GT) || !type.compare(tokenType::LT)||
       !type.compare(tokenType::GET) || !type.compare(tokenType::LET)||
       !type.compare(tokenType::EQUAL)){
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
    if(checkNextTokenType(1,tokenType::IDENTIFIER.c_str())){
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
    if(checkNextTokenType(1,tokenType::LBRACKET.c_str())){
        consumeNextToken(tokenType::LBRACKET);
        node->child.push_back(parseAddtiveExp());
        consumeNextToken(tokenType::RBRACKET);
    }
    
    return node;
}

void Parser::printT(TreeNode *t,char* outpath) {
    ostream& out = cout;
    printTree(t,0);
}

void Parser::printTree(TreeNode *root, int depth) {
    for(int d = depth; d > 0; d--)
        cout<<"\t";
    cout<<TreeNodeType::getTrueType(root->nodekind)<<endl;
    int i;
    for(i = 0; i < root->child.size(); i++){
        printTree(root->child[i],depth+1);
    }
}























