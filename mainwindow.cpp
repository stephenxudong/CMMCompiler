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
    string fileName;
    fileName = QFileDialog::getOpenFileName(this,"","", tr("CMM(*.c--)")).toStdString();
    string content = "";
    QString text = QString::fromStdString(content);

    ui->textEdit->setPlainText(text);
    ui->textEdit->raise();
    auto tokens = lexcial(fileName.c_str(),"out.txt");
    auto root = parse(tokens,nullptr);
}

void MainWindow::on_textEdit_textChanged()
{

}


