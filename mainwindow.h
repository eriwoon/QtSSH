#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "xzdatebase.h"
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
    XZDatebase* datebase;

private slots:
    void hostInfo();
    void aboutInfo();
    void executable();
};

#endif // MAINWINDOW_H
