#include <iostream>
#include <string>
#include "util/Util.h"
#include "mainwindow.h"
#include <QApplication>

using namespace std;
int main(int argc, char** argv) {
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}

