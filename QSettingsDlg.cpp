#include "QSettingsDlg.h"
#include "ui_QSettingsDlg.h"

QSettingsDlg::QSettingsDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QSettingsDlg)
{
    ui->setupUi(this);
}

QSettingsDlg::~QSettingsDlg()
{
    delete ui;
}
