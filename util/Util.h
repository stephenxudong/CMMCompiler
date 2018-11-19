//
// Created by Stephen Liao on 2018/9/12.
//

#ifndef CMMINTEPRETER_FILEUTIL_H
#define CMMINTEPRETER_FILEUTIL_H
#include <vector>
#include <string>
#include "../type/Token.h"
#include "../type/TreeNode.h"
using namespace std;
vector<char> toCharArray(const string& s);
vector<string> readToLines(const char *filepath);
vector<string> split(string& str, const string& delim);
string& replace_all(string& str, const string& old_value, const string& new_value);
vector<string> preProcessing(vector<string>& lines);
void eliminate(vector<Token>& tokens);
static void printErrors(void);

#endif //CMMINTEPRETER_FILEUTIL_H
