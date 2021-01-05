#include <thread>

#include <QApplication>
#include <QWebEnginePage>
#include <QDebug>

#include "JlWebTranslatorWrapper.h"

using namespace std::chrono_literals;

class JlWebTranslatorWrapperPrivate : public QObject
{
    Q_OBJECT

    QString translate(const QString &text2Translate, const QString &outputLang, const QString &inputLang);

    bool load(QWebEnginePage* page, const QUrl &url);
    QVariant runJavaScript(QWebEnginePage *page, const QString &script);

    friend class JlWebTranslatorWrapper;
};

#include "JlWebTranslatorWrapper.moc"

bool JlWebTranslatorWrapperPrivate::load(QWebEnginePage* page, const QUrl &url)
{
    using namespace std::chrono_literals;

    bool answered = false;
    bool result = false;

    connect(page, &QWebEnginePage::loadFinished, [&](bool ok)
    {
        result = ok;
        answered = true;
    });

    page->load(url);

    while (!answered)
    {
        QApplication::processEvents();
        std::this_thread::sleep_for(10ms);
    }

    return result;
}

QVariant JlWebTranslatorWrapperPrivate::runJavaScript(QWebEnginePage *page, const QString &script)
{
    using namespace std::chrono_literals;

    QVariant retValue;
    bool answered = false;

    page->runJavaScript(script, [&](const QVariant &v)
    {
        retValue = v;
        //qDebug() << v.toString();
        answered = true;
    });

    while (!answered)
    {
        QApplication::processEvents();
        std::this_thread::sleep_for(10ms);
    }

    return retValue;
}

QString JlWebTranslatorWrapperPrivate::translate(const QString &text2Translate, const QString &outputLang, const QString &inputLang)
{
    std::unique_ptr<QWebEnginePage> page(new QWebEnginePage);

    load(page.get(), QUrl("https://translate.google.com/?q=" + QUrl::toPercentEncoding(text2Translate) + "&sl=" + inputLang + "&tl=" + outputLang));

    QString query = " \
        let all = document.getElementsByTagName(\"*\"); \
        let result = \"\"; \
        for (var i = 0, max = all.length; i < max; ++i) \
        { \
            if (all[i].hasAttribute(\"data-initial-value\")) \
            { \
                result = all[i].innerText;\
                break; \
            } \
        } \
        result \
        ";

    QVariant retValue = runJavaScript(page.get(), query);
    return retValue.isNull() || !retValue.isValid() ? "" : retValue.toString();
}

// JlWebTranslatorWrapper
JlWebTranslatorWrapper::JlWebTranslatorWrapper()
    : pimpl()
{

}

JlWebTranslatorWrapper::~JlWebTranslatorWrapper()
{

}

QString JlWebTranslatorWrapper::translate(const QString &text2Translate, const QString &outputLang, const QString &inputLang)
{
    return d->translate(text2Translate, outputLang, inputLang);
}
