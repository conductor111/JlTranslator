#include <set>

#include <QFileInfo>

#include "LangsAvailable.h"

#include "QSettingsDlg.h"
#include "ui_QSettingsDlg.h"

QSettingsDlg::QSettingsDlg(QSettings &settings, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::QSettingsDlg)
    , m_settings{ settings }
{
    ui->setupUi(this);

    initSlots();
    initSlotsInline();

    if (m_settings.format() == QSettings::IniFormat && !QFileInfo::exists(m_settings.fileName()))
    {
        m_settings.setValue(c_inputLangKey, "en");
        m_settings.setValue(c_outputLangsKey, "it");
    }

    initControls();
}

QSettingsDlg::~QSettingsDlg()
{
    delete ui;
}

void QSettingsDlg::initSlots()
{
    connect(ui->pushButton_Cancel, &QPushButton::clicked, this, &QDialog::reject);
}

void QSettingsDlg::initSlotsInline()
{
    connect(ui->pushButton_LangMoveToEnabled, &QPushButton::clicked, this, [this]()
    {
        moveListWidgetItem(ui->listWidget_LangsAvailable, ui->listWidget_LangsEnabled);
    });
    connect(ui->pushButton_LangMoveToAvailable, &QPushButton::clicked, this, [this]()
    {
        moveListWidgetItem(ui->listWidget_LangsEnabled, ui->listWidget_LangsAvailable);
    });
}

void QSettingsDlg::initControls()
{
    std::set<QString> outputLangs;
    for (auto it : m_settings.value(c_outputLangsKey).toString().splitRef(c_outputLangsDelimiter))
    {
        outputLangs.emplace(it.toString());
    }
    QString inputLang = m_settings.value(c_inputLangKey).toString();

    ui->comboBox_InputLang->clear();
    int currentInputLangInd = -1;
    for (auto it : langsAvailable())
    {
        bool isCurrentInputLang = it.first == inputLang;
        if (isCurrentInputLang)
        {
            currentInputLangInd = ui->comboBox_InputLang->count();
        }

        QString langDesc = it.second + " - " + it.first;
        ui->comboBox_InputLang->addItem(langDesc, it.first);

        if (isCurrentInputLang)
        {
            continue;
        }

        auto item = new QListWidgetItem{ langDesc };
        item->setData(Qt::UserRole, it.first);

        (outputLangs.count(it.first) == 0 ? ui->listWidget_LangsAvailable : ui->listWidget_LangsEnabled)->addItem(item);
    }

    if (currentInputLangInd >= 0)
    {
        ui->comboBox_InputLang->setCurrentIndex(currentInputLangInd);
    }

    ui->comboBox_InputLang->model()->sort(0);
}

void QSettingsDlg::moveListWidgetItem(QListWidget* listFrom, QListWidget* listTo)
{
    for (auto item : listFrom->selectedItems())
    {
        listTo->addItem(listFrom->takeItem(listFrom->row(item)));
    }
}
