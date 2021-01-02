#ifndef JLDTDFILEPROCESSOR_H
#define JLDTDFILEPROCESSOR_H

#include <memory>

#include <QString>

#include "pimpl.h"

class JlDtdFileProcessor : public pimpl<class JlDtdFileProcessorPrivate>
{
public:
    JlDtdFileProcessor();
    ~JlDtdFileProcessor();

    void processDtdFile(const QString &dtdFilePath, const QString &dtdOutputDirPath, const QStringList &outputLangs, const QString &inputLang = "en");
};

#endif // JLDTDFILEPROCESSOR_H
