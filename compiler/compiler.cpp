//
// Created by Stephen Liao on 2018/9/19.
//

#include "compiler.h"
#include <QtDebug>
#include <QString>
#include <util/Util.h>
using namespace std;

vector<Token> Compiler::lexcial(vector<string>& lines,vector<string>& errors){
    // 去除单行注释
    preProcessing(lines);
    Lexical lex;
    // 词法分析
    lex.parse(lines);
    auto err = lex.geterrors();
    auto tokens = lex.getTokens();
    if(err.size()!=0)
        continueFlag=false;
    errors = err;
    return tokens;
}

TreeNode* Compiler::parse(vector<Token>& tokens,vector<string>& errors){
    //首先去掉多余的一些符号的token
    eliminate(tokens);
    Parser p(tokens);
# ifdef DEBUG
    for(auto token : tokens){
        QString s;
        s = s.fromStdString(token.toString());
        qDebug()<<s;
    }
# endif
    TreeNode* root = p.parse();
    errors = p.error();
    return root;
}

void Compiler::compile(const char *filepath, MainWindow& w,const char *output)
{
//    auto tokens = lexcial(filepath,"out.txt");
//    TreeNode* root = nullptr;
//    if(continueFlag){
//        root = parse(tokens);
//    }
//    else{
//        cout<<"Some Lexical Error, Program Exit"<<endl;
//        exit(1);
//    }
//    if(continueFlag){
//        semantic(root,w);
//    }else{
//        cout<<"Some Grammer Error, Program Exit"<<endl;
//        exit(1);
//    }

}

void Compiler::semantic(TreeNode *root,MainWindow& w)
{
    Semantic s(root,w);
    s.run();
}

