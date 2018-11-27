#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QFileDialog"
#include "compiler/compiler.h"
#include "compiler/Parser.h"
using namespace compiler;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_toolButton_clicked()
{
    QFileDialog* fileDialog = new QFileDialog(this);
    fileDialog->setWindowTitle(tr("Open File"));//设置文件保存对话框的标题
    fileDialog->setAcceptMode(QFileDialog::AcceptOpen);//设置文件对话框为打开模式
    fileDialog->setFileMode(QFileDialog::AnyFile);
    fileDialog->setViewMode(QFileDialog::Detail);//文件以详细的形式显示，显示文件名，大小，创建日期等信息；
    fileDialog->setDirectory("../");//设置文件对话框打开时初始打开的位置
    if(fileDialog->exec()){
        QString path = fileDialog->selectedFiles()[0];
        auto tokens = lexcial(path.toStdString().c_str(),"out.txt");
        auto root = parse(tokens,nullptr);
    }
}
