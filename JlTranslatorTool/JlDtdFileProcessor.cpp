#include <assert.h>
#include <map>
#include <stack>

#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QMessageBox>
#include <QApplication>
#include <QRegularExpression>

#include "StringUtil.h"
#include "JlWebTranslatorWrapper.h"

#include "JlDtdFileProcessor.h"

using namespace jlStringUtil;

QString addEndSlashIfNeed(const QString& pathDir)
{
    if (!pathDir.isEmpty() && pathDir[pathDir.length() - 1] != QDir::separator())
    {
        return  pathDir + QDir::separator();
    }

    return pathDir;
}

// JlDtdFileProcessorPrivate
class JlDtdFileProcessorPrivate
{
    static inline QString c_entityErrorValue = "### CANNOT BE TRANSLATED AUTOMATICALLY ###";

    struct TEntity
    {
        bool rawData;
        QString id;
        QString value;
    };
    typedef std::vector<TEntity> EntityCollection;

    JlWebTranslatorWrapper m_webTranslatorWrapper;
    bool m_isProcessing = false;
    bool m_needBreak = false;
    bool m_translateError = false;
    log_cb_t log;

    void processDtdFile(const QString &dtdFilePath, const QString &dtdOutputDirPath, const QStringList &outputLangs, const QString &inputLangconst, const JlDtdFileProcessor::StrStrMap &outputLangPathAliases);

    static bool isValidStartOfName(const QString name);
    bool parseDocType(const QString &data, EntityCollection &entityCollection);
    void translateDtdFile(const EntityCollection &entityCollection, const QString &dtdOutputDirPath, const QString &dtdFileName, const QString &outputLang, const QString &inputLang, const JlDtdFileProcessor::StrStrMap &outputLangPathAliases);
    void writeEntity(QTextStream &stream, const QString &id, const QString &value);
    const QString& outputLangPath(const QString& outputLang, const JlDtdFileProcessor::StrStrMap &outputLangPathAliases);

    static QString& fixEntityRef(QString &str);

    friend class JlDtdFileProcessor;

public:
    JlDtdFileProcessorPrivate(const log_cb_t &log_cb) : log{ log_cb } {}
};

bool JlDtdFileProcessorPrivate::isValidStartOfName(const QString name)
{
    return name[0] == '_' || name[0].isLetter();
}

QString& JlDtdFileProcessorPrivate::fixEntityRef(QString &str)
{
    //QRegularExpression re(R"(&\s*#\s*([xX]?[abcdefABCDEF\d]+)\s*;)");
    QRegularExpression re(R"(&\s*(#?)\s*([lLgG]t|[aA]mp|[nN]bsp|[qQ]uot|[aA]pos|[cC]ent|[pP]ound|[yY]en|[eE]uro|[cC]opy|[rR]eg|[xX]?[abcdefABCDEF\d]+)\s*;)");
    QRegularExpressionMatchIterator matchIterator = re.globalMatch(str);

    std::stack<QRegularExpressionMatch> matches;
    while (matchIterator.hasNext())
    {
        matches.push(matchIterator.next());
    }

    while (!matches.empty())
    {
        const QRegularExpressionMatch &match = matches.top();
        QString captured = match.captured(2);
        captured[0] = captured[0].toLower();
        str.replace(match.capturedStart(), match.capturedLength(), "&" + match.captured(1) + captured + ";");
        matches.pop();
    }

    return str;
}

bool JlDtdFileProcessorPrivate::parseDocType(const QString &data, EntityCollection &entityCollection)
{
    // <!ENTITY Name "txtValue">
    QString pattern{ "\\s+([\\-\\.\\:\\w]+)\\s*\"(.*)\"\\s*>" };
    const int groupCount = 2;

    bool atOnce = false;
    for (auto part : data.trimmed().split("<!ENTITY"))
    {
        tstring t = _TQ(part);
        part = _TQ(rtrim(t));
        if (part.isEmpty() || part[part.size() - 1] != '>')
        {
            continue;
        }

        QRegExp re(pattern);

        if (re.indexIn(part) == -1)
        {
            continue;
        }

        QStringList capturedTexts = re.capturedTexts();
        if (capturedTexts.count() < groupCount + 1)
        {
            continue;
        }

        if (!isValidStartOfName(capturedTexts[1]))
        {
            return false;
        }

        atOnce = true;
        entityCollection.emplace_back( TEntity{ false, capturedTexts[1], capturedTexts[2] } );
    }

    return atOnce;
}

void JlDtdFileProcessorPrivate::writeEntity(QTextStream &stream, const QString &id, const QString &value)
{
    stream << QString("<!ENTITY %1 \"%2\">\n").arg(id).arg(value);
}

const QString& JlDtdFileProcessorPrivate::outputLangPath(const QString& outputLang, const JlDtdFileProcessor::StrStrMap &outputLangPathAliases)
{
    auto it = outputLangPathAliases.find(outputLang);
    if (it != outputLangPathAliases.end())
    {
        return it->second;
    }

    return outputLang;
}

void JlDtdFileProcessorPrivate::translateDtdFile(const EntityCollection &entityCollection, const QString &dtdOutputDirPath, const QString &dtdFileName, const QString &outputLang, const QString &inputLang, const JlDtdFileProcessor::StrStrMap &outputLangPathAliases)
{
    assert(QFileInfo::exists(dtdOutputDirPath) && !outputLang.isEmpty() && !inputLang.isEmpty());

    QString outputDir = addEndSlashIfNeed(dtdOutputDirPath) + outputLangPath(outputLang, outputLangPathAliases);
    if (!QFileInfo::exists(outputDir))
    {
        QDir().mkpath(outputDir);
    }

    QString dtdFileFullPath = addEndSlashIfNeed(outputDir) + dtdFileName;
    if (QFile::exists(dtdFileFullPath))
    {
        auto res = QMessageBox::question(qApp->activeWindow(), QObject::tr("Question"),
            QObject::tr("File \"") + dtdFileFullPath + QObject::tr("\" already exists. Overwrite it?"),
            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        if (res == QMessageBox::No || res == QMessageBox::Cancel)
        {
            m_needBreak = res == QMessageBox::Cancel;
            log(m_needBreak ? QObject::tr("Canceled by user\n") :
                (QObject::tr("File \"") + dtdFileFullPath + QObject::tr("\" has been skipped.\n")), Qt::GlobalColor::black, QFont::Bold);
            return;
        }

        if (!QFileInfo(dtdFileFullPath).permission(QFile::WriteUser))
        {
            auto res = QMessageBox::warning(qApp->activeWindow(), QObject::tr("Question"),
                QObject::tr("No permission to overwrite file \"") + dtdFileFullPath + QObject::tr("\". Skip it?"),
                QMessageBox::Yes | QMessageBox::Cancel);
            m_needBreak = res == QMessageBox::Cancel;
            log(m_needBreak ? QObject::tr("Canceled by user\n") :
                (QObject::tr("File \"") + dtdFileFullPath + QObject::tr("\" has been skipped.\n")), Qt::GlobalColor::black, QFont::Bold);
            return;
        }
    }

    QFile dtdFile(dtdFileFullPath);
    dtdFile.open(QFile::WriteOnly | QFile::Text);
    QTextStream stream(&dtdFile);

    log("\n[" + outputLang + "]\n" + dtdFile.fileName() + "\n", Qt::GlobalColor::black, QFont::Bold);

    for (TEntity entity : entityCollection)
    {
        if (m_needBreak)
        {
            break;
        }

        if (entity.rawData)
        {
            stream << entity.value;            
            continue;
        }

        log(entity.id + ":", Qt::GlobalColor::black, QFont::Normal);
        QString outputText = m_webTranslatorWrapper.translate(entity.value, outputLang, inputLang);
        bool error = outputText.isEmpty();
        if (error)
        {
            m_translateError = true;
            outputText = c_entityErrorValue;
        }
        else
        {
            fixEntityRef(outputText);
        }
        writeEntity(stream, entity.id, outputText);
        log(outputText + "\n", error ? Qt::GlobalColor::red : Qt::GlobalColor::blue, QFont::Normal);
    }
}

void JlDtdFileProcessorPrivate::processDtdFile(const QString &dtdFilePath, const QString &dtdOutputDirPath, const QStringList &outputLangs, const QString &inputLang, const JlDtdFileProcessor::StrStrMap &outputLangPathAliases)
{
    if (m_isProcessing)
    {
        throw std::runtime_error(_TQ(QObject::tr("JlDtdFileProcessorPrivate: already in process")));
    }

    assert(QFile::exists(dtdFilePath) && QFileInfo::exists(dtdOutputDirPath) && !outputLangs.isEmpty() && !inputLang.isEmpty());

    QFile dtdFile(dtdFilePath);
    dtdFile.open(QFile::ReadOnly | QFile::Text);

    EntityCollection entityCollection;
    for (QByteArray line = dtdFile.readLine(); !line.isEmpty(); line = dtdFile.readLine())
    {
        if (!parseDocType(line, entityCollection))
        {
            entityCollection.emplace_back(TEntity{ true, "", line });
        }
    }    

    m_isProcessing = true;
    m_translateError = false;
    for (QString outputLang : outputLangs)
    {
        translateDtdFile(entityCollection, dtdOutputDirPath, QFileInfo(dtdFile.fileName()).fileName(), outputLang, inputLang, outputLangPathAliases);
        if (m_needBreak)
        {
            break;
        }
    }

    if (m_needBreak)
    {
        m_needBreak = false;
        log(QObject::tr("Canceled by user\n"), Qt::GlobalColor::black, QFont::Bold);
    }
    else
    {
        log(QObject::tr("Done\n"), Qt::GlobalColor::black, QFont::Bold);
    }

    if (m_translateError)
    {
        QMessageBox::warning(qApp->activeWindow(), QObject::tr("Warning"),
            QObject::tr("Some values cannot be translated automatically. You can find them in output files by:\n\n  ") +
            c_entityErrorValue + QObject::tr("\n\nstring."));
    }

    m_isProcessing = false;
}

// JlDtdFileProcessor
JlDtdFileProcessor::JlDtdFileProcessor(const log_cb_t &log_cb)
    : pimpl{ log_cb }
{

}

JlDtdFileProcessor::~JlDtdFileProcessor()
{

}

bool JlDtdFileProcessor::isProcessing() const
{
    return d->m_isProcessing;
}

void JlDtdFileProcessor::processDtdFile(const QString &dtdFilePath, const QString &dtdOutputDirPath, const QStringList &outputLangs, const QString &inputLang, const StrStrMap &outputLangPathAliases)
{
    d->processDtdFile(dtdFilePath, dtdOutputDirPath, outputLangs, inputLang, outputLangPathAliases);
}

void JlDtdFileProcessor::breakProcess()
{
    if (!d->m_isProcessing || d->m_needBreak)
    {
        return;
    }

    d->m_needBreak = true;
}

bool JlDtdFileProcessor::isProcessBroken() const
{
    return d->m_needBreak;
}
