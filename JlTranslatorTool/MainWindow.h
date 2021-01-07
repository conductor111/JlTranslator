#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>

#include "JlDtdFileProcessor.h"

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
    QSettings m_settings;

    static inline const QString c_inputFilePathKey = "InputFilePath";
    static inline const QString c_outputDirPathKey = "OutputDirPath";

    JlDtdFileProcessor m_jlDtdFileProcessor;

    virtual void closeEvent(QCloseEvent *event);

    void initSlots();
    void initSlotsInline();
    void initControls();
    void onSelectInputFile();
    void onSelectOutputDir();
    void onProcess_Break();

    bool prepareDtdFileProcessingParams(QString &dtdFilePath, QString &dtdOutputDirPath, QStringList &outputLangs, QString &inputLang, JlDtdFileProcessor::StrStrMap &outputLangPathAliases);

    void log(const QString& text, const QColor &textColor, int fontWeight);
};

#endif // MAINWINDOW_H
