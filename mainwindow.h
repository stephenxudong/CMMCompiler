#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolButton>
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

    QToolButton *getOkButton() const;

private slots:
    void on_toolButton_clicked();

private:
    Ui::MainWindow *ui;
    QTextEdit* textEdit;
    QToolButton* okButton;
};

#endif // MAINWINDOW_H
