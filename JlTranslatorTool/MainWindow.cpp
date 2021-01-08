#include <memory>
#include <thread>
#include <iostream>

#include <QMessageBox>
#include <QFileDialog>

#include "LangsAvailable.h"
#include "StringUtil.h"
#include "QSettingsDlg.h"

#include "MainWindow.h"
#include "ui_MainWindow.h"

using namespace jlStringUtil;

#ifdef Q_OS_WIN
extern Q_CORE_EXPORT int qt_ntfs_permission_lookup;
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_settings{ QSettings::IniFormat, QSettings::UserScope, "JL", "JlTranslatorTool" }
    , m_jlDtdFileProcessor{ [this](const QString& text, const QColor &textColor, int fontWeight) { log(text, textColor, fontWeight); } }
{
    ui->setupUi(this);

#ifdef Q_OS_WIN
    qt_ntfs_permission_lookup++;
#endif

    initSlots();
    initSlotsInline();
    initControls();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);

    if (m_jlDtdFileProcessor.isProcessing())
    {
        m_jlDtdFileProcessor.breakProcess();
    }
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

    connect(ui->pushButton_Process_Break, &QPushButton::clicked, this, [this]()
    {
        onProcess_Break();
    });
}

void MainWindow::initControls()
{
    auto getPathFromSettings = [this](const QString &settingsKey) -> QString
    {
        QString savedPath = m_settings.value(settingsKey).toString();
        return QFileInfo::exists(savedPath) ? savedPath : "";
    };

    ui->lineEdit_InputFile->setText(getPathFromSettings(c_inputFilePathKey));
    ui->lineEdit_OutputDir->setText(getPathFromSettings(c_outputDirPathKey));
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

void MainWindow::onProcess_Break()
{
    if (m_jlDtdFileProcessor.isProcessing())
    {
        if (m_jlDtdFileProcessor.isProcessBroken())
        {
            return;
        }

        m_jlDtdFileProcessor.breakProcess();
        ui->pushButton_Process_Break->setText(tr("Process"));
        return;
    }

    auto enableControls = [this](bool enabled)
    {
        ui->pushButton_Settings->setEnabled(enabled);
        ui->pushButton_SelectInputFile->setEnabled(enabled);
        ui->pushButton_SelectOutputDir->setEnabled(enabled);
    };

    QString dtdFilePath, dtdOutputDirPath, inputLang;
    QStringList outputLangs;
    JlDtdFileProcessor::StrStrMap outputLangPathAliases;
    if (!prepareDtdFileProcessingParams(dtdFilePath, dtdOutputDirPath, outputLangs, inputLang, outputLangPathAliases))
    {
        return;
    }

    ui->pushButton_Process_Break->setText(tr("Break"));
    enableControls(false);
    try
    {
        m_jlDtdFileProcessor.processDtdFile(dtdFilePath, dtdOutputDirPath, outputLangs, inputLang, outputLangPathAliases);
        ui->pushButton_Process_Break->setText(tr("Process"));
    }
    catch (const std::exception& ex)
    {
        ui->pushButton_Process_Break->setText(tr("Process"));
        QMessageBox::warning(nullptr, tr("Error"), tr("Cannot execute:\n") + _TQ(ex.what()));
    }
    enableControls(true);
}

bool MainWindow::prepareDtdFileProcessingParams(QString &dtdFilePath, QString &dtdOutputDirPath, QStringList &outputLangs, QString &inputLang, JlDtdFileProcessor::StrStrMap &outputLangPathAliases)
{
    bool res = true;
    QString errDesc;

    auto checkPath = [&errDesc](const QString &path, const QString &pathDesc, QFile::Permission permToCheck) -> bool
    {
        if (path.isEmpty())
        {
            errDesc += pathDesc + tr(" is empty\n");
            return false;
        }

        QFileInfo fi(path);
        if (!fi.exists())
        {
            errDesc += pathDesc + " \"" + path + tr("\" doesn't exist\n");
            return false;
        }

        if (!fi.permission(permToCheck))
        {
            errDesc += pathDesc + " \"" + path + tr("\" - required file permissions are missing\n");
            return false;
        }

        return true;
    };

    auto checkLang = [&errDesc](const QString &lang, const QString &langDesc) -> bool
    {
        if (lang.isEmpty())
        {
            errDesc += langDesc + tr(" is empty\n");
            return false;
        }

        if (langsAvailable().count(lang) == 0)
        {
            errDesc += langDesc + " \"" + lang + tr("\" is invalid\n");
            return false;
        }

        return true;
    };

    dtdFilePath = ui->lineEdit_InputFile->text();
    dtdOutputDirPath = ui->lineEdit_OutputDir->text();
    inputLang = m_settings.value(QSettingsDlg::c_inputLangKey).toString();

    res &= checkPath(dtdFilePath, tr("Input file path"), QFile::ReadUser);
    res &= checkPath(dtdOutputDirPath, tr("Output directory path"), QFile::WriteUser);
    res &= checkLang(inputLang, tr("Input language"));

    for (QString outputLang : m_settings.value(QSettingsDlg::c_outputLangsKey).toString().split(QSettingsDlg::c_outputLangsDelimiter))
    {
        if (outputLangs.contains(outputLang))
        {
            continue;
        }

        res &= checkLang(outputLang, tr("Output language"));

        if (!res)
        {
            continue;
        }

        outputLangs.append(outputLang);
        QString alias = m_settings.value(QSettingsDlg::c_outputLangPathAliasesGroup + outputLang).toString();
        if (!alias.isEmpty())
        {
            outputLangPathAliases[outputLang] = alias;
        }
    }


    if (res)
    {
        return true;
    }

    QMessageBox::warning(nullptr, tr("Error"), tr("Cannot execute:\n") + errDesc);

    return false;
}

void MainWindow::log(const QString& text, const QColor &textColor, int fontWeight)
{
    if (!QMainWindow::isWindow())
    {
        return;
    }

    ui->textEdit_Log->moveCursor(QTextCursor::MoveOperation::End);
    ui->textEdit_Log->setTextColor(textColor);
    ui->textEdit_Log->setFontWeight(fontWeight);
    //ui->textEdit_Log->append(text);    
    ui->textEdit_Log->insertPlainText(text + "\n");
    ui->textEdit_Log->moveCursor(QTextCursor::MoveOperation::End);
    ui->textEdit_Log->ensureCursorVisible();

    //std::cerr << text.toStdString() << std::endl;
}
