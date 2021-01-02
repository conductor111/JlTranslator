#include <memory>
#include <thread>
#include <iostream>

#include <QMessageBox>
#include <QFileDialog>

#include "QSettingsDlg.h"

#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->pushButton_Exit, &QPushButton::clicked, this, [this]()
    {
        QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                        "/home",
                                                        QFileDialog::ShowDirsOnly
                                                        | QFileDialog::DontResolveSymlinks);

//        std::chrono::time_point<std::chrono::steady_clock> b = std::chrono::steady_clock::now();
//        auto val = m_jlWebTranslatorWrapper.translate("Put it on the table!", "ru");
//        std::chrono::time_point<std::chrono::steady_clock> e = std::chrono::steady_clock::now();
//        std::cout << "f2 - " << std::chrono::duration_cast<std::chrono::milliseconds>(e - b).count() << " msec" << std::endl;
//        QMessageBox::information(this, "Info", val);
    });

    connect(ui->pushButton_Settings, &QPushButton::clicked, this, [this]()
    {
        QSettingsDlg dlg(this);
        dlg.exec();
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}
