#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include "xzdatebase.h"
#include "xzseqdock.h"
#include <QLabel>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    XZDatebase* db;
    XZSeqDock* dock;
    QTextEdit* textEdit;
    void initialTextEdit();
    QLabel* status;
private slots:
    void hostInfo();
    void aboutInfo();
    void executable();
    void sequence();
    void setoutput(QString);
    void clearOutput();
    void setStatusBarTip(QString);
};


#endif // MAINWINDOW_H
