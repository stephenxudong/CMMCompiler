//
// Created by Stephen Liao on 2018/9/19.
//

#include "Lexical.h"
#include "../util/Util.h"
#include "../type/tokentype.h"

/**
 * this function is designed to be a parser conducting lexical analysis
 * identifier type:
 *
 * 	    运算符：+、-、*、/、=、==、<>、<、>
 * 	    分隔符：(、)、{、}、[、]、,、;
 *	    注释符：//、/~*、*~/
 *      标识符：要求必须是以字母开头，由字母、数字和下划线组成，且不能以下划线结尾的
 *      字符串
 *      数字：整数和实数(排除以多个零开头的情况，如0009、00.009)
 *
 */

void Lexical::parse(const vector<std::string> &lines) {
    for(int i = 0; i < lines.size(); i++){
        // 这样的想法是 避免根据换行符来计算行数
        if(lines[i].size()>0)
            this->parseLine(lines[i], i+1);
    }
}
void Lexical::parseLine(const string& currLine, int lineNum) {
    auto line = string(currLine);
    line.append("\n");
    vector<char> chars = toCharArray(line);
    int begin = 0, end = 0;
    int state = 0;
    
    for(int i = 0; i < chars.size(); i++){
        char ch = chars.at(i);
        string temp;
        string c;
        if (!comment){
            if(this->validate(ch)){
                // 有效字符
                switch (state) {
                    case 0:
                        if(ch==' ')continue;
                        // 处理格式符号
                        else if(ch=='\t'){
                            this->tokens.push_back(Token("制表符",valueOf(ch), lineNum, i+1));
                            state = 0;
                        }
                        else if(ch=='\r'){
                            this->tokens.push_back(Token("回车符",valueOf(ch), lineNum, i+1));
                            state = 0;
                        }
                        else if(ch=='\n'){
                            this->tokens.push_back(Token("换行符",valueOf(ch), lineNum, i+1));
                            state = 0;
                        }
                        
                        // 处理分割符号
                        else if(ch==';'){
                            this->tokens.push_back(Token(TokenType::SEMICOLON, ";", lineNum, i+1));
                        }
                        else if(ch=='('){
                            this->tokens.push_back(Token(TokenType::LPAREN,"(", lineNum, i+1));
                            state = 0;
                        }
                        else if(ch==')'){
                            this->tokens.push_back(Token(TokenType::RPAREN,")", lineNum, i+1));
                            state = 0;
                        }
                        else if(ch=='['){
                            this->tokens.push_back(Token(TokenType::LBRACKET,"[", lineNum, i+1));
                            state = 0;
                        }
                        else if(ch==']'){
                            this->tokens.push_back(Token(TokenType::RBRACKET,"]", lineNum, i+1));
                            state = 0;
                        }
                        else if(ch=='{'){
                            this->tokens.push_back(Token(TokenType::LBRACE,"{", lineNum, i+1));
                            state = 0;
                        }
                        else if(ch=='}'){
                            this->tokens.push_back(Token(TokenType::RBRACE,"}", lineNum, i+1));
                            state = 0;
                        }
                        else if(ch==','){
                            this->tokens.push_back(Token(TokenType::COMMA, ",", lineNum, i+1));
                            state = 0;
                        }
                        else if(ch=='"'){
                            // 字符串
                            begin = i+1;
                            state = 3;
                        }
                        else if(isDigit(ch)){
                            // 数字
                            begin = i;
                            state = 1;
                        }
                        else if(isLetter(ch)){
                            // 标识符或者关键字
                            begin = i;
                            state = 2;
                        }
                        // 处理操作符
                        else if(ch=='+')
                            state = 4;
                        else if(ch=='-')
                            state = 5;
                        else if(ch=='*')
                            state = 6;
                        else if(ch=='/')
                            state = 7;
                        else if(ch=='%')
                            state = 8;
                        else if(ch=='=')
                            state = 9;
                        else if(ch=='>')
                            state = 10;
                        else if(ch=='<')
                            state = 11;
                        break;
                    case 1:
                        // 处理数字
                        if(isDigit(ch) || ch=='.') state = 1;
                        else{
                            if(isLetter(ch)){
                                // 数字的格式错误,结尾包含字符
                                errnum++;
                                string errmsg;
                                errmsg.append("    ERROR:第 ")
                                .append(to_string(lineNum))
                                .append(" 行,第 ")
                                .append(to_string(begin+1))
                                .append(" 列：")
                                .append("数字或者标识符格式错误\n");
                                this->errors.push_back(errmsg);
                            }else{
                                end = i;
                                string num = line.substr(begin,end-begin);
                                i--;
                                if(num.find(".")==string::npos){
                                    // 不是浮点数
                                    if(matchInteger(num)){
                                        this->tokens.push_back(Token(TokenType::LITERAL_INT, num, lineNum, begin+1));
                                    }else{
                                        errnum++;
                                        string errmsg;
                                        errmsg.append("    ERROR:第 ")
                                        .append(to_string(lineNum))
                                        .append(" 行,第 ")
                                        .append(to_string(begin+1))
                                        .append(" 列：")
                                        .append(num).append("是非法整数\n");
                                        this->errors.push_back(errmsg);
                                    }
                                }else{
                                    if(matchReal(num)){
                                        this->tokens.push_back(Token(TokenType::LITERAL_REAL, num, lineNum, begin+1));
                                    }else{
                                        errnum++;
                                        string errmsg;
                                        errmsg.append("    ERROR:第 ")
                                        .append(to_string(lineNum))
                                        .append(" 行,第 ")
                                        .append(to_string(begin+1))
                                        .append(" 列：")
                                        .append(num).append("是非法浮点数\n");
                                        this->errors.push_back(errmsg);
                                    }
                                }
                            }
                            state = 0;
                        }
                        break;
                        
                    case 2:
                        // 处理标识符或者关键字
                        if(isLetter(ch)||isDigit(ch))state = 2;
                        else{
                            end = i;
                            string val = line.substr(begin,end-begin);
                            i--;
                            if(matchKey(val)){
                                this->tokens.push_back(Token(TokenType::KEYWORD, val, lineNum, begin+1));
                                state = 0;
                            }
                            else if(mathchIdentifier(val)){
                                if(!val.compare("true"))
                                    this->tokens.push_back(Token(TokenType::LITERAL_TRUE, val, lineNum, begin+1));
                                else if(!val.compare("false"))
                                    this->tokens.push_back(Token(TokenType::LITERAL_FALSE, val, lineNum, begin+1));
                                else
                                    this->tokens.push_back(Token(TokenType::IDENTIFIER, val, lineNum, begin+1));
                                state = 0;
                            }
                            else{
                                errnum++;
                                string errmsg;
                                errmsg.append("    ERROR:第 ")
                                .append(to_string(lineNum))
                                .append(" 行,第 ")
                                .append(to_string(begin+1))
                                .append(" 列：")
                                .append(val).append("是非法标识符或关键字\n");
                                this->errors.push_back(errmsg);
                                state = 0;
                            }
                        }
                       
                        break;
                        
                    case 3:
                        if(ch=='"'){
                            end = i;
                            string val = line.substr(begin,end-begin);
                            this->tokens.push_back(Token(TokenType::STRING, val, lineNum, begin+1));
                            state = 0;
                        }
                        else if(i ==line.length()-1){
                            string val = line.substr(begin);
                            errnum++;
                            string errmsg;
                            errmsg.append("    ERROR:第 ")
                            .append(to_string(lineNum))
                            .append(" 行,第 ")
                            .append(to_string(begin+1))
                            .append(" 列：")
                            .append(val).append("缺少\",非法字符串\n");
                            this->errors.push_back(errmsg);
                        }
                        break;
                    case 4:
                        this->tokens.push_back(Token(TokenType::PLUS, "+", lineNum, i));
                        i--;
                        state = 0;
                        break;
                    case 5:
                        temp = tokens[tokens.size()-1].getType();
                        c = tokens[tokens.size()-1].getText();
                        if((temp==TokenType::IDENTIFIER||temp==TokenType::LITERAL_REAL
                                ||temp==TokenType::LITERAL_INT||c==")"||c=="]")){
                            this->tokens.push_back(Token(TokenType::MINUS, "-", lineNum, i));
                            i--;
                            state = 0;
                        }
                        else{
                            //if(isDigit(ch) && !isDigit(chars.at(i-3)) && !isLetter(chars.at(i-3)))
                            //表示的是负数,即一元运算符
                            i--;//现需要回退
                            begin = i;
                            state = 1;
                        }
                        break;
                    case 6:
                        this->tokens.push_back(Token(TokenType::MULTI, "*", lineNum, i));
                        i--;
                        state = 0;
                        break;
                    case 7:
                        // 由于预处理的时候已经去掉了单行注释,所以此处只用考虑除号或者多行注释
                        if (ch == '*') {
                            begin = i + 1;
                            comment = true;
                        }
                        else{
                            this->tokens.push_back(Token(TokenType::DIVIDE, "/", lineNum, i));
                            i--;
                            state = 0;
                        }
                        break;
                    case 8:
                        this->tokens.push_back(Token(TokenType::MOD, "%", lineNum, i));
                        i--;
                        state = 0;
                        break;
                    case 9:
                        // 两个连续的等号是equal，一个等号是assign
                        if (ch == '=') {
                            this->tokens.push_back(Token(TokenType::EQUAL, "=", lineNum, i));
                        }else{
                            this->tokens.push_back(Token(TokenType::ASSIGN, "=", lineNum, i));
                            i--;
                            
                        }
                        state = 0;
                        break;
                    case 10:
                        this->tokens.push_back(Token(TokenType::GT, ">", lineNum, i));
                        i--;
                        state = 0;
                        break;
                    case 11:
                        if(ch=='>'){
                            this->tokens.push_back(Token(TokenType::NEQUAL, "<>", lineNum, i));
                        }
                        else{
                            this->tokens.push_back(Token(TokenType::LT, "<", lineNum, i));
                            i--;
                        }
                        state = 0;
                        break;
                        
                    default:
                        break;
                }
                
            }
            else{
                 errnum++;
                string errmsg;
                errmsg.append("    ERROR:第 ")
                .append(to_string(lineNum))
                .append(" 行,第 ")
                .append(to_string(i+1))
                .append(" 列：")
                .append("无法识别的符号\n");
                this->errors.push_back(errmsg);
            }
        }
        else{
            if(ch=='*')
                state = -1;// 注释即将结束
            else if(ch=='/'&&state==-1){
                // 注释结束
                state = 0;
                comment = false;
                this->tokens.push_back(Token("多行注释", "COMMENT", lineNum, i));
            }
        }
    }
   
}

bool Lexical::matchInteger(const string &s) {
    // r2 为多0的情况，需要排除
    return regex_match(s, regex("^-?\\d+$"))
    && !regex_match(s, regex("^-?0{1,}\\d+$"));
}

bool Lexical::matchReal(const string &s) {
    // 需要排除小数点前面唯一的0超多两次的情况
    return regex_match(s, regex("^(-?\\d+)(\\.\\d+)$"))
    && !regex_match(s, regex("^(-?0{2,})(\\.\\d+)+$"));
}


bool Lexical::mathchIdentifier(const string &s) {
    return regex_match(s,regex("^\\w+$")) && !(s[s.size()-1]=='_')
    && regex_match(s.substr(0, 1),regex("[A-Za-z]"));
}
/**
 * 保留字：if、else、while、read、write、int、bool、real
 * @param s :some key you want to match
 * @return isMatched or not
 */
bool Lexical::matchKey(const string &s) {
    return !s.compare("if") || !s.compare("else") || !s.compare("read") ||
           !s.compare("write")|| !s.compare("int") || !s.compare("bool") ||
           !s.compare("real")  || !s.compare("while")||!s.compare("string")
            ||!s.compare("for");
}

bool Lexical::validate(const char ch) { 
    return (ch == '(' || ch == ')' || ch == ';' || ch == '{'
     || ch == '}' || ch == '[' || ch == ']' || ch == ','
     || ch == '+' || ch == '-' || ch == '*' || ch == '/'
     || ch == '=' || ch == '<' || ch == '>' || ch == '"'
     || isLetter(ch) || isDigit(ch) || ch == '.'
     || ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t');
}




