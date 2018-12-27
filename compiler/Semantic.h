#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "type/SymbolTable.h"
#include "type/TreeNode.h"
#include "util/Thread.h"
#include "type/TreeNodeType.h"
#include "mainwindow.h"
#include <regex>
//多线程？？
#include <QThread>
#include <QMutex>
#include <QSemaphore>
#include <QObject>
class Semantic: public QThread
{
    Q_OBJECT
public:
    explicit Semantic(TreeNode* root,QObject* parent=nullptr);
    ~Semantic()=default;

    void pause();
    void resume();
    void setUserInput(const string &value);
    void setRoot(TreeNode* root);
protected:
    void run();

private:
    SymbolTable table;//符号表
    TreeNode* root;//抽象语法树
    int level;//语义分析标识符的作用域
    //错误处理
    string errorInfo;
    int errorNum = 0;
    vector<string> errors;
    string userInput;
    MainWindow* w;
    QMutex m_mutex;
    bool flag;

private:

    void error(string err, int line);
    bool matchInteger(const string &s);
    bool matchReal(const string &s);

    void statement(TreeNode* root);
    void declare(TreeNode* node);
    void assign(TreeNode* node);
    void ifStmt(TreeNode* node);
    void forStmt(TreeNode* node);
    void whileStmt(TreeNode* node);
    void readStmt(TreeNode* node);
    void writeStmt(TreeNode* node);
    bool condition(TreeNode* node);
    string expression(TreeNode* node);
    string array(TreeNode* node, int size);

    bool checkID(TreeNode* node, int level);

    string readInput();

signals:
    void output(const QString&);



};
#endif // SEMANTIC_H
