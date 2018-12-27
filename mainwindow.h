#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QPushButton>
#include <string>
#include <vector>
#include <type/Token.h>
#include <type/TreeNode.h>
#include <QStandardItem>
#include <QTreeWidget>
#include <QList>
using std::string;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QTextEdit *getTextEdit() const;
    QPushButton *getOkButton() const;
    string getInput();

private slots:
    void on_textEdit_textChanged();
    void on_inputText_textChanged();
    void displayOutput(const QString& s);

private:
    Ui::MainWindow *ui;
    QTextEdit* textEdit;
    QPushButton* okButton;
    // source code
    vector<string> sourceCode;
    // semantic thread
    QThread* t;
    // semantic input
    string input;
    string fileName;
    // tokens after Lexical
    vector<Token> tokens;
    // abstract Syntax Tree
    TreeNode* root;
    // errors
    vector<string> errors;

private:
    void drawTree(TreeNode* root);
    QList<QStandardItem*> drawNode(TreeNode* node);
};

#endif // MAINWINDOW_H
