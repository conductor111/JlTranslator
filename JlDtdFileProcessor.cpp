#include <assert.h>
#include <map>

#include <QFile>
#include <QDir>
#include <QTextStream>

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
    struct TEntity
    {
        bool rawData;
        QString id;
        QString value;
    };
    typedef std::vector<TEntity> EntityCollection;

    JlWebTranslatorWrapper m_webTranslatorWrapper;

    void processDtdFile(const QString &dtdFilePath, const QString &dtdOutputDirPath, const QStringList &outputLangs, const QString &inputLang);

    static bool isValidStartOfName(const QString name);
    bool parseDocType(const QString &data, EntityCollection &entityCollection);
    void translateDtdFile(const EntityCollection &entityCollection, const QString &dtdOutputDirPath, const QString &dtdFileName, const QString &outputLang, const QString &inputLang);
    void writeEntity(QTextStream &stream, const QString &id, const QString &value);

    friend class JlDtdFileProcessor;
};

bool JlDtdFileProcessorPrivate::isValidStartOfName(const QString name)
{
    return name[0] == '_' || name[0].isLetter();
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

void JlDtdFileProcessorPrivate::translateDtdFile(const EntityCollection &entityCollection, const QString &dtdOutputDirPath, const QString &dtdFileName, const QString &outputLang, const QString &inputLang)
{
    assert(QFileInfo::exists(dtdOutputDirPath) && !outputLang.isEmpty() && !inputLang.isEmpty());

    QString outputDir = addEndSlashIfNeed(dtdOutputDirPath) + outputLang;
    if (!QFileInfo::exists(outputDir))
    {
        QDir().mkpath(outputDir);
    }

    assert(!QFile::exists(addEndSlashIfNeed(outputDir) + dtdFileName));

    QFile dtdFile(addEndSlashIfNeed(outputDir) + dtdFileName);
    dtdFile.open(QFile::WriteOnly | QFile::Text);
    QTextStream stream(&dtdFile);

    for (TEntity entity : entityCollection)
    {
        if (entity.rawData)
        {
            stream << entity.value;
            continue;
        }

        writeEntity(stream, entity.id, m_webTranslatorWrapper.translate(entity.value, outputLang, inputLang));
    }
}

void JlDtdFileProcessorPrivate::processDtdFile(const QString &dtdFilePath, const QString &dtdOutputDirPath, const QStringList &outputLangs, const QString &inputLang)
{
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

    for (QString outputLang : outputLangs)
    {
        translateDtdFile(entityCollection, dtdOutputDirPath, QFileInfo(dtdFile.fileName()).fileName(), outputLang, inputLang);
    }
}

// JlDtdFileProcessor
JlDtdFileProcessor::JlDtdFileProcessor()
    : pimpl()
{

}

JlDtdFileProcessor::~JlDtdFileProcessor()
{

}

void JlDtdFileProcessor::processDtdFile(const QString &dtdFilePath, const QString &dtdOutputDirPath, const QStringList &outputLangs, const QString &inputLang)
{
    d->processDtdFile(dtdFilePath, dtdOutputDirPath, outputLangs, inputLang);
}
