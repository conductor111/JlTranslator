#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "JlWebTranslatorWrapper.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    JlWebTranslatorWrapper m_jlWebTranslatorWrapper;
};

#endif // MAINWINDOW_H
