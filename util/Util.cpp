//
// Created by Stephen Liao on 2018/9/12.
//

#include "Util.h"
#include <iostream>
#include <fstream>
#include <cassert>
#include <cstdio>
#include <cstring>
#include "../type/tokentype.h"
#include <unistd.h>
// 将一个字符串转化成字符序列
vector<char> toCharArray(const string& s) {
    vector<char> sequence;
    auto iter = s.begin();
    while(iter != s.end()){
        sequence.push_back(*iter);
        iter++;
    }
    return sequence;
}

// 返回的vector的每一个分量是原来文本中的每一行
vector<string> readToLines(const char *filepath) {
    fstream fin;
    string line;
    fin.open(filepath,ios::in);
    if(!fin)
        cout<<"open file error"<<endl;
    vector<string> res;
    assert(fin.is_open());
    while(getline(fin,line)){
        res.push_back(line);
    }
    fin.close();
    return res;
}
// 根据delim来分割字符串
vector<string> split(string& str, const string& delim){
    vector<string> res;
    if("" == str) return res;
    //先将要切割的字符串从string类型转换为char*类型
    char * strs = new char[str.length() + 1] ; //不要忘了
    strcpy(strs, str.c_str());

    char * d = new char[delim.length() + 1];
    strcpy(d, delim.c_str());

    char *p = strtok(strs, d);
    while(p) {
        string s = p; //分割得到的字符串转换为string类型
        res.push_back(s); //存入结果数组
        p = strtok(NULL, d);
    }

    return res;
}
string& replace_all(string& str, const string& old_value, const string& new_value)
{
    while(true)   {
        string::size_type   pos(0);
        if(   (pos=str.find(old_value))!=string::npos   )
            str.replace(pos,old_value.length(),new_value);
        else   break;
    }
    return   str;
}

/**
 去掉多余的空格，制表符，去掉单行注释
 
 @param lines 待处理的每一行文本
 @return 处理之后的文本
 */
vector<string> preProcessing(vector<string>& lines){
    vector<string> ans;
    for(auto line = lines.begin(); line != lines.end(); line++){
        // 首先去掉多余的空格和制表符
        //(*line) = replace_all(*line, "  ", " ");
        //(*line) = replace_all(*line, "\t", "");
        for (auto i = (*line).begin(); i != (*line).end(); i++) {
            if(*i=='/' && i+1 != (*line).end() && *(i+1)=='/'){
                (*line).erase(i, (*line).end());
                break;
            }
            // 多行注释
//            else if(*i=='/' && i+1 != (*line).end() && *(i+1)=='*'){
//                string s(*line);
//                s.erase(i, (*line).end());
//                ans.push_back(s);
//                flag = true;
//                break;
//            }
        }
//        while(flag && (*line))
        
    }
    return lines;
}


void eliminate(vector<Token>& tokens){
    for(auto i = tokens.begin(); i != tokens.end();){
        auto type = (*i).getType();
        if(!type.compare(TokenType::KEYWORD)){
            string t((*i).getText());
            (*i).setType(t);
        }
        if(!type.compare("多行注释") || !type.compare("制表符")
           || !type.compare("换行符") || !type.compare("回车符"))
             i = tokens.erase(i);
        else i++;
    }
}


static void printErrors(void){
    printf("代码结尾中缺少关键部分");
}

