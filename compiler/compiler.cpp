//
// Created by Stephen Liao on 2018/9/19.
//

#include "compiler.h"
using namespace std;
static bool continueFlag = true;
vector<Token> Compiler::lexcial(const char* infile,const char* outpath){
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
        continueFlag=false;
        for(auto i : vec){
            cout<<i<<endl;
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

TreeNode* Compiler::parse(vector<Token>& tokens,char* outpath){
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
    else{
        continueFlag=false;
        for(string& i:p.error())
            cout<<i<<endl;
    }
    return root;
}

void Compiler::compile(const char *filepath, MainWindow& w,const char *output)
{
    auto tokens = lexcial(filepath,"out.txt");
    TreeNode* root = nullptr;
    if(continueFlag){
        root = parse(tokens);
    }
    else{
        cout<<"Some Lexical Error, Program Exit"<<endl;
        exit(1);
    }
    if(continueFlag){
        semantic(root,w);
    }else{
        cout<<"Some Grammer Error, Program Exit"<<endl;
        exit(1);
    }

}

void Compiler::semantic(TreeNode *root,MainWindow& w)
{
    Semantic s(root,w);
    s.run();
}

