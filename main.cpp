#include <iostream>
#include <string>
#include "compiler/compiler.h"
#include "compiler/Parser.h"
#include "util/Util.h"
#include "mainwindow.h"
#include <QApplication>

using namespace compiler;
using namespace std;
int main(int argc, char** argv) {
//    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();
//    auto rtnval = a.exec();
    auto tokens = lexcial("test/while.cmm","out.txt");
    auto root = parse(tokens,nullptr);
//    cout<<string("MULTI").c_str()<<endl;
//    cout<<string("*").c_str()<<endl;
    return 0;
}

