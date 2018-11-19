//
// Created by Stephen Liao on 2018/9/12.
//

#ifndef CMMINTEPRETER_TOKEN_H
#define CMMINTEPRETER_TOKEN_H

#include <string>
using namespace std;

class Token {
private:
    string type;// 值为tag类中的值
    string  text;
    //行数
    int line;
    //位置
    int position;// start from 1 not 0 
public:
    Token(const string &type, const string &text, int line, int position);
    const string getType() const;
    const string getText() const;
    int getLine() const;
    void setLine(int line);
    int getPosition() const;
    void setPosition(int position);
    void setText(string &text);
    void setType(string &val);
    string toString();

};


#endif //CMMINTEPRETER_TOKEN_H
