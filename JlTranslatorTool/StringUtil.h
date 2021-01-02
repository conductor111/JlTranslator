#ifndef STRINGUTIL_H
#define STRINGUTIL_H

#include <string>
#include <regex>

#include <QVariant>

namespace jlStringUtil
{

#ifdef UNICODE
    typedef std::wstring tstring;
    #define _std_string_to_qstring_ QString::fromStdWString
    #define _qstring_to_std_string_ toStdWString
    #define to_tstring std::to_wstring
#else
    typedef std::string tstring;
    #define _std_string_to_qstring_ QString::fromStdString
    #define _qstring_to_std_string_ toStdString
    #define to_tstring std::to_string
#endif

template<typename T> struct _TQ
{
    _TQ(const T& str) : m_str(str) {}

    operator tstring() const { return m_str; }
    operator QString() const { return m_str; }

    auto c_str()
    {
        m_tmp = this->operator tstring();
        return m_tmp.c_str();
    }

    operator QVariant() const
    {
        return QVariant(operator QString());
    }

private:
    const T& m_str;
    tstring m_tmp;
};

template<>
inline _TQ<QString>::operator tstring() const
{
    return m_str._qstring_to_std_string_();
}

template<>
inline _TQ<tstring>::operator QString() const
{
    return _std_string_to_qstring_(m_str);
}

///////////////////////////////////////////////

static inline std::string &ltrim(std::string &s, std::function<int(int)> currentTrim = nullptr)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
        currentTrim ? currentTrim : [](int c){return !::isspace(c);}));
    return s;
}

static inline std::string &rtrim(std::string &s, std::function<int(int)> currentTrim = nullptr)
{
    s.erase(std::find_if(s.rbegin(), s.rend(),
        currentTrim ? currentTrim : [](int c){return !::isspace(c);}).base(), s.end());
    return s;
}

static inline std::string &trim(std::string &s, std::function<int(int)> currentTrim = nullptr)
{
    return ltrim(rtrim(s, currentTrim), currentTrim);
}

static inline std::vector<std::string> split(const std::string& input, const std::string& regex)
{
    // passing -1 as the submatch index parameter performs splitting
    std::regex re(regex);
    std::sregex_token_iterator first{ input.begin(), input.end(), re, -1 }, last;
    return{ first, last };
}

};
#endif // STRINGUTIL_H
