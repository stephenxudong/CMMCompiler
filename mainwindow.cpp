#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QFileDialog"
#include "compiler/compiler.h"
#include "compiler/Parser.h"
#include "compiler/Semantic.h"
#include <QMutex>

extern QWaitCondition semaphore;
using namespace Compiler;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->textEdit=ui->textEdit;
    connect(ui->pushButton,&QPushButton::clicked,this,[=]{
        string fileName;
        fileName = QFileDialog::getOpenFileName(this,"../","", tr("")).toStdString();
        string content = "";
        QString text = QString::fromStdString(content);

        ui->textEdit->setPlainText(text);
        ui->textEdit->raise();
        auto tokens = lexcial(fileName.c_str(),"out.txt");
        TreeNode* root = nullptr;
        if(continueFlag){
            root = parse(tokens);
        }
        else{
            cout<<"Some Lexical Error, Program Exit"<<endl;
            exit(1);
        }
        if(continueFlag){
            this->t = new Semantic(root,*this);
            t->start();
        }else{
            cout<<"Some Grammer Error, Program Exit"<<endl;
            exit(1);
        }


    });
    connect(ui->okBtn,&QPushButton::clicked,this,[=]{
        input = ui->inputText->toPlainText().toStdString();
        //seman = dynamic_cast<QThread*>(seman);
    //    compile::semaphore.notify_all();
        Semantic* s = dynamic_cast<Semantic*>(this->t);
        s->setUserInput(input);
        s->resume();
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

//void MainWindow::on_toolButton_clicked()
//{
//    string fileName;
//    fileName = QFileDialog::getOpenFileName(this,"../","", tr("")).toStdString();
//    string content = "";
//    QString text = QString::fromStdString(content);

//    ui->textEdit->setPlainText(text);
//    ui->textEdit->raise();
//    this->t = new Compiler(fileName.c_str(),this);
//    t->start();
//}



QTextEdit *MainWindow::getTextEdit() const
{
    return textEdit;
}



//void MainWindow::on_okBtn_clicked()
//{
//    //input = ui->in
//    input = ui->inputText->toPlainText().toStdString();
//    //seman = dynamic_cast<QThread*>(seman);
//    compile::semaphore.notify_all();
//    Semantic* s = dynamic_cast<Semantic*>(this->t);
//    s->setUserInput(input);
//    s->resume();
//}

string MainWindow::getInput()
{
    return input;
}
