#ifndef QSETTINGSDLG_H
#define QSETTINGSDLG_H

#include <QDialog>

namespace Ui {
class QSettingsDlg;
}

class QSettingsDlg : public QDialog
{
    Q_OBJECT

public:
    explicit QSettingsDlg(QWidget *parent = nullptr);
    ~QSettingsDlg();

private:
    Ui::QSettingsDlg *ui;
};

#endif // QSETTINGSDLG_H
