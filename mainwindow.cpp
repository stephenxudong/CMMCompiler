#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QFileDialog"
#include "compiler/compiler.h"
#include "compiler/Parser.h"
#include "compiler/Semantic.h"
#include <QMutex>
#include <iostream>
#include <sstream>
extern bool Compiler::continueFlag;
using namespace Compiler;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QColor color;
    ui->fileText->setTextColor(Qt::black);
    ui->fileText->setFont(QFont(tr("Consolas"), 14));
    ui->inputText->setTextColor(Qt::black);
    this->ui->lexicalOutText->setTextColor(Qt::black);
    ui->outTextEdit->setTextColor(Qt::black);

    connect(ui->pushButton,&QPushButton::clicked,this,[=]{
        string fileName;
        fileName = QFileDialog::getOpenFileName(this,"../","", tr("")).toStdString();
        QString text;
        this->sourceCode= readToLines(fileName.c_str());
        if(ui->fileText->toPlainText()!="")
            ui->fileText->clear();
        for(auto line : sourceCode){
            ui->fileText->append(text.fromStdString(line));
        }
    });
    connect(ui->lexicalBtn,&QPushButton::clicked,this,[=]{
        QString infile = this->ui->fileText->toPlainText();
        stringstream f(infile.toStdString());
        string line;
        this->sourceCode.clear();
        while(getline(f,line)){
            this->sourceCode.push_back(line);
        }
        if(this->sourceCode.size()==0){
            ui->outTextEdit->setText("没有打开文件，请重试");
            return;
        }
        this->tokens = lexcial(this->sourceCode,errors);
        QString s;
        if(errors.size()!=0){
            if(ui->lexicalOutText->toPlainText()!="")
                ui->lexicalOutText->clear();
            ui->lexicalOutText->setText("存在一些词法错误:\n");
            for(auto i : errors)
                ui->lexicalOutText->append(s.fromStdString(i));
        }
        else{
            if(ui->lexicalOutText->toPlainText()!="")
                ui->lexicalOutText->clear();
            for(auto i : tokens)
                ui->lexicalOutText->append(s.fromStdString(i.toString()).append("\n"));
        }

    });
    connect(ui->syntaxBtn,&QPushButton::clicked,this,[=]{
        if(this->tokens.size()==0){
            ui->outTextEdit->setText("请先进行词法分析");
            return;
        }
        QString s;
        this->root = parse(tokens,errors);
        if(errors.size()!=0){
            ui->outTextEdit->setText("存在一些语法错误:\n");
            for(auto i : errors)
                ui->outTextEdit->append(s.fromStdString(i));
        }
        this->drawTree(this->root);
    });
    connect(ui->semanticBtn,&QPushButton::clicked,this,[=]{
        if(this->root==nullptr){
            ui->outTextEdit->setText("请先进行语法分析");
            return;
        }
        this->t = new Semantic(this->root);
        connect(t,SIGNAL(output(const QString&)),this,SLOT(displayOutput(const QString&)));
        t->start();
    });

    connect(ui->okBtn,&QPushButton::clicked,this,[=]{
        input = ui->inputText->toPlainText().toStdString();
        Semantic* s = dynamic_cast<Semantic*>(this->t);
        s->setUserInput(input);
        s->resume();
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}


QTextEdit *MainWindow::getTextEdit() const
{
    return textEdit;
}

string MainWindow::getInput()
{
    return input;
}

void MainWindow::drawTree(TreeNode *root)
{
    if(root==nullptr)return;
    QTreeView* tree = new QTreeView();
    QStandardItemModel* model = new QStandardItemModel(ui->treeView);
    model->setHorizontalHeaderLabels(QStringList()<<QStringLiteral("structure"));
    QList<QStandardItem*> items,childrenItems;
    QStandardItem* item = new QStandardItem(QString::fromStdString(root->getNodekind()));

    for(size_t i = 0; i < root->childNum(); i++){
        childrenItems = drawNode(root->getChild(i));
        if(childrenItems.size()!=0)
            item->appendRow(childrenItems);
    }
    items.append(item);
    model->appendRow(items);
    ui->treeView->setModel(model);
//    tree->show();
}

QList<QStandardItem *> MainWindow::drawNode(TreeNode *node)
{
    QStandardItem* item = new QStandardItem(QString::fromStdString(node->getNodekind()));
      QList<QStandardItem*>items, childrenItems;
      for(size_t i = 0; i < node->childNum();i++){
          childrenItems = drawNode(node->getChild(i));
          if(childrenItems.size()!=0)
              item->appendRow(childrenItems);
      }
      items.append(item);
      return items;
}

void MainWindow::on_inputText_textChanged()
{
    ui->inputText->setTextColor(Qt::black);
}

void MainWindow::displayOutput(const QString &s)
{
    this->ui->outTextEdit->append(s);
}

void MainWindow::on_outTextEdit_textChanged()
{
    this->ui->outTextEdit->setTextColor(Qt::black);
}

void MainWindow::on_lexicalOutText_textChanged()
{
    this->ui->lexicalOutText->setTextColor(Qt::black);
}

void MainWindow::on_fileText_textChanged()
{
    this->ui->fileText->setTextColor(Qt::black);
}
