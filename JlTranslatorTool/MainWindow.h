#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>

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
    QSettings m_settings;

    static inline const QString c_inputFilePathKey = "InputFilePath";
    static inline const QString c_outputDirPathKey = "OutputDirPath";

    JlWebTranslatorWrapper m_jlWebTranslatorWrapper;

    void initSlots();
    void initSlotsInline();
    void initControls();
    void onSelectInputFile();
    void onSelectOutputDir();
};

#endif // MAINWINDOW_H
