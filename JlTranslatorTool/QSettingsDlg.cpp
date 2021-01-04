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

        m_settings.beginGroup(c_outputLangPathAliasesGroup);
        m_settings.setValue("en", "en-US");
        m_settings.setValue("es", "es-ES");
        m_settings.setValue("id", "id-ID");
        m_settings.setValue("pt", "pt-BR");
        m_settings.setValue("sv", "sv-SE");
        m_settings.endGroup();
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
    connect(ui->comboBox_InputLang, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index)
    {
        QVariant userData = ui->comboBox_InputLang->itemData(index);

        QListWidget* listWidget = ui->listWidget_LangsAvailable;
        auto listIndexes = listWidget->model()->match(listWidget->model()->index(0, 0), Qt::UserRole, userData);
        if (listIndexes.isEmpty())
        {
            listWidget = ui->listWidget_LangsEnabled;
            listIndexes = listWidget->model()->match(listWidget->model()->index(0, 0), Qt::UserRole, userData);
        }
        if (listIndexes.isEmpty())
        {
            return;
        }

        // remove
        QListWidgetItem* item = listWidget->takeItem(listIndexes[0].row());

        // insert
        (m_onChangeInputLangAddCurrentToEnabled ? ui->listWidget_LangsEnabled : ui->listWidget_LangsAvailable)->addItem(m_currentInputLangItem.release());
        m_currentInputLangItem.reset(item);

        m_onChangeInputLangAddCurrentToEnabled = listWidget == ui->listWidget_LangsEnabled;
    });
    connect(ui->pushButton_Ok, &QPushButton::clicked, this, [this]()
    {
        m_settings.setValue(c_inputLangKey, ui->comboBox_InputLang->currentData().toString());

        QString outputLangs;

        for (auto item : ui->listWidget_LangsEnabled->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard))
        {
            outputLangs += (outputLangs.isEmpty() ? "" : c_outputLangsDelimiter) + item->data(Qt::UserRole).toString();
        }
        m_settings.setValue(c_outputLangsKey, outputLangs);

        accept();
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
        QString langDesc = it.second + " - " + it.first;

        bool isCurrentInputLang = it.first == inputLang;
        if (isCurrentInputLang)
        {
            currentInputLangInd = ui->comboBox_InputLang->count();
            m_currentInputLangItem = std::make_unique<QListWidgetItem>(langDesc);
            m_currentInputLangItem->setData(Qt::UserRole, it.first);
        }

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
