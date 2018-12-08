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
    this->textEdit=ui->textEdit;
    this->okButton = ui->okBtn;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_toolButton_clicked()
{
    string fileName;
    fileName = QFileDialog::getOpenFileName(this,"","", tr("")).toStdString();
    string content = "";
    QString text = QString::fromStdString(content);

    ui->textEdit->setPlainText(text);
    ui->textEdit->raise();
    compile(fileName.c_str(),*this);
}

QToolButton *MainWindow::getOkButton() const
{
    return okButton;
}

QTextEdit *MainWindow::getTextEdit() const
{
    return textEdit;
}




