#include <iostream>
#include <string>
#include "compiler/compiler.h"
#include "compiler/Parser.h"
#include "util/Util.h"
using namespace compiler;
using namespace std;
int main() {
    auto tokens = lexcial("test/while.cmm","out.txt");
    auto root = parse(tokens,NULL);
//    cout<<string("MULTI").c_str()<<endl;
//    cout<<string("*").c_str()<<endl;
    return 0;
}
