#ifndef QSETTINGSDLG_H
#define QSETTINGSDLG_H

#include <memory>

#include <QDialog>
#include <QSettings>
#include <QListWidget>

namespace Ui {
class QSettingsDlg;
}

class QSettingsDlg : public QDialog
{
    Q_OBJECT

public:
    explicit QSettingsDlg(QSettings &settings, QWidget *parent = nullptr);
    ~QSettingsDlg();

private:
    Ui::QSettingsDlg *ui;
    QSettings& m_settings;
    std::unique_ptr<QListWidgetItem> m_currentInputLangItem;
    bool m_onChangeInputLangAddCurrentToEnabled = false;

    static inline const QString c_outputLangsDelimiter = ":";
    static inline const QString c_inputLangKey = "Langs/Input";
    static inline const QString c_outputLangsKey = "Langs/Output";
    static inline const QString c_outputLangPathAliasesGroup = "OutputLangPathAliases/";

    void initSlots();
    void initSlotsInline();
    void initControls();
    void moveListWidgetItem(QListWidget* listFrom, QListWidget* listTo);
};

#endif // QSETTINGSDLG_H
