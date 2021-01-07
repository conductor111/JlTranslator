#ifndef JLDTDFILEPROCESSOR_H
#define JLDTDFILEPROCESSOR_H

#include <memory>
#include <map>
#include <functional>

#include <QString>
#include <QColor>

#include "pimpl.h"

typedef std::function<void(const QString& text, const QColor &textColor, int fontWeight)> log_cb_t;

class JlDtdFileProcessor : public pimpl<class JlDtdFileProcessorPrivate, log_cb_t>
{
public:    
    JlDtdFileProcessor(const log_cb_t &log_cb);
    ~JlDtdFileProcessor();

    bool isProcessing() const;
    typedef std::map<QString, QString> StrStrMap;
    void processDtdFile(const QString &dtdFilePath, const QString &dtdOutputDirPath, const QStringList &outputLangs, const QString &inputLang, const StrStrMap &outputLangPathAliases);
    void breakProcess();
    bool isProcessBroken() const;
};

#endif // JLDTDFILEPROCESSOR_H
