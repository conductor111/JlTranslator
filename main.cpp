#include <memory>
#include <vector>
#include <iostream>

//#include <QCoreApplication>
#include <QApplication>
#include <QMessageBox>

#include <QUrl>
#include <QUrlQuery>

#include "MainWindow.h"

#include "test.h"

int main(int argc, char *argv[])
{
    //QCoreApplication a(argc, argv);
    QApplication a(argc, argv);

    try
    {
        //test();
    }
    catch(const std::exception &ex)
    {
        QMessageBox::warning(nullptr, "Error", ex.what());
    }

    MainWindow w;
    w.show();

    return a.exec();
}
