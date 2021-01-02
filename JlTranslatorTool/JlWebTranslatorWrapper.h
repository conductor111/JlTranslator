#ifndef JLWEBTRANSLATORWRAPPER_H
#define JLWEBTRANSLATORWRAPPER_H

#include <memory>

#include <QString>

#include "pimpl.h"

class JlWebTranslatorWrapper : public pimpl<class JlWebTranslatorWrapperPrivate>
{
public:
    JlWebTranslatorWrapper();
    ~JlWebTranslatorWrapper();
    QString translate(const QString &text2Translate, const QString &outputLang, const QString &inputLang = "en");
};

#endif // JLWEBTRANSLATORWRAPPER_H
