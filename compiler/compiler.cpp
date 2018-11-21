//
// Created by Stephen Liao on 2018/9/19.
//

#include "compiler.h"
using namespace std;
vector<Token> compiler::lexcial(const char* infile,const char* outpath){
    auto lines = readToLines(infile);
    // 去除单行注释
    preProcessing(lines);
    Lexical lex;
    // 词法分析
    lex.parse(lines);
    auto vec = lex.geterrors();
    auto tokens = lex.getTokens();
    fstream fout;
    fout.open(outpath,ios::out);
    if(!vec.empty()){
        cout<<"Some Errors! Please Check!"<<endl;
        for(auto i : vec){
            fout<<i<<endl;
        }
    }
    else {
        cout<<"Great, No Lexical Errors!"<<endl;
        if(outpath){
            // write to file
            for(auto token : tokens)
                fout<<token.toString()<<endl;
        }
        else{
            // print to std::out
            for(auto token : tokens)
                cout<<token.toString()<<endl;
        }
    }
    return tokens;
}

TreeNode* compiler::parse(vector<Token>& tokens,char* outpath){
    //首先去掉多余的一些符号的token
    eliminate(tokens);
    Parser p(tokens);
    TreeNode* root = p.parse();
    fstream fout;
    fout.open(outpath,ios::out);
    if(p.error().size()==0){
        cout<<"Great, No Syntax Errors!"<<endl;
        p.printTree(root, 0);
    }
    return root;
}
