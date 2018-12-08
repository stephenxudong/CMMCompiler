#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QPushButton>
#include <string>
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


private:
    Ui::MainWindow *ui;
    QTextEdit* textEdit;
    QPushButton* okButton;
    string input;
    QThread* t;
    bool continueFlag;
};

#endif // MAINWINDOW_H
