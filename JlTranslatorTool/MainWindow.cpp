#include <memory>
#include <thread>
#include <iostream>

#include <QMessageBox>
#include <QFileDialog>

#include "QSettingsDlg.h"

#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_settings{ QSettings::IniFormat, QSettings::UserScope, "JL", "JlTranslatorTool" }
{
    ui->setupUi(this);

    initSlots();
    initSlotsInline();
    initControls();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initSlots()
{
    connect(ui->pushButton_Exit, &QPushButton::clicked, this, &QMainWindow::close);
}

void MainWindow::initSlotsInline()
{
    connect(ui->pushButton_Settings, &QPushButton::clicked, this, [this]()
    {
        QSettingsDlg dlg(m_settings, this);
        dlg.exec();
    });

    connect(ui->pushButton_SelectInputFile, &QPushButton::clicked, this, [this]()
    {
        onSelectInputFile();
    });

    connect(ui->pushButton_SelectOutputDir, &QPushButton::clicked, this, [this]()
    {
        onSelectOutputDir();
    });
}

void MainWindow::initControls()
{
    ui->lineEdit_InputFile->setText(m_settings.value(c_inputFilePathKey).toString());
    ui->lineEdit_OutputDir->setText(m_settings.value(c_outputDirPathKey).toString());
}

void MainWindow::onSelectInputFile()
{
    QString initPath = m_settings.value(c_inputFilePathKey).toString();
    if (initPath.isEmpty())
    {
        initPath = QDir::homePath();
    }

    QString filePath = QFileDialog::getOpenFileName(this, tr("Select input file"), initPath, "DTD-files (*.dtd)");

    if (filePath.isEmpty())
    {
        return;
    }

    ui->lineEdit_InputFile->setText(filePath);
    m_settings.setValue(c_inputFilePathKey, filePath);
}

void MainWindow::onSelectOutputDir()
{
    QString initPath = m_settings.value(c_outputDirPathKey).toString();
    if (initPath.isEmpty())
    {
        initPath = QDir::homePath();
    }

    QString dirPath = QFileDialog::getExistingDirectory(this, tr("Select output directory"),
        initPath, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (dirPath.isEmpty())
    {
        return;
    }

    ui->lineEdit_OutputDir->setText(dirPath);
    m_settings.setValue(c_outputDirPathKey, dirPath);
}
