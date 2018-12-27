#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QFileDialog"
#include "compiler/compiler.h"
#include "compiler/Parser.h"
#include "compiler/Semantic.h"
#include <QMutex>

extern bool Compiler::continueFlag;
using namespace Compiler;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->textEdit=ui->textEdit;
    this->input = "";
    QColor color;
    ui->fileText->setTextColor(Qt::black);
    ui->inputText->setTextColor(Qt::black);
    ui->textEdit->setTextColor(Qt::black);
    connect(ui->pushButton,&QPushButton::clicked,this,[=]{
        string fileName;
        fileName = QFileDialog::getOpenFileName(this,"../","", tr("")).toStdString();
        QString text;
        this->sourceCode= readToLines(fileName.c_str());
        for(auto line : sourceCode){
            ui->fileText->append(text.fromStdString(line).append("\n"));
        }
    });
    connect(ui->lexicalBtn,&QPushButton::clicked,this,[=]{
        if(this->sourceCode.size()==0){
            ui->textEdit->setText("没有打开文件，请重试");
            return;
        }
        this->tokens = lexcial(this->sourceCode,errors);
        QString s;
        if(errors.size()!=0){
            ui->textEdit->setText("存在一些词法错误:\n");
            for(auto i : errors)
                ui->textEdit->append(s.fromStdString(i));
        }
        else{
            for(auto i : tokens)
                ui->textEdit->append(s.fromStdString(i.toString()).append("\n"));
        }

    });
    connect(ui->syntaxBtn,&QPushButton::clicked,this,[=]{
        if(this->tokens.size()==0){
            ui->textEdit->setText("请先进行词法分析");
            return;
        }
        QString s;
        this->root = parse(tokens,errors);
        if(errors.size()!=0){
            ui->textEdit->setText("存在一些语法错误:\n");
            for(auto i : errors)
                ui->textEdit->append(s.fromStdString(i));
        }
        this->drawTree(this->root);
    });
    connect(ui->semanticBtn,&QPushButton::clicked,this,[=]{
        if(this->root==nullptr){
            ui->textEdit->setText("请先进行语法分析");
            return;
        }
        this->t = new Semantic(root,*this);
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
    QStandardItem* item = new QStandardItem(QString::fromStdString(root->getContent()));

    for(size_t i = 0; i < root->childNum(); i++){
        childrenItems = drawNode(root->getChild(i));
        if(childrenItems.size()==0)
            item->appendRow(childrenItems);
    }
    items.append(item);
    model->appendRow(items);
    ui->treeView->setModel(model);
//    tree->show();
}

QList<QStandardItem *> MainWindow::drawNode(TreeNode *node)
{
    QStandardItem* item = new QStandardItem(QString::fromStdString(node->getContent()));
      QList<QStandardItem*>items, childrenItems;
      for(size_t i = 0; i < node->childNum();i++){
          childrenItems = drawNode(node->getChild(i));
          if(childrenItems.size()==0)
              item->appendRow(childrenItems);
      }
      items.append(item);
      return items;
}

void MainWindow::on_textEdit_textChanged()
{
    ui->textEdit->setTextColor(Qt::black);
}

void MainWindow::on_inputText_textChanged()
{
    ui->inputText->setTextColor(Qt::black);
}
