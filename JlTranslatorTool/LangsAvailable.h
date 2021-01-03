#ifndef _LANGS_AVAILABLE_2020_01_02_
#define _LANGS_AVAILABLE_2020_01_02_

#include <map>

#include <QString>

typedef std::map<QString, QString> LangsCollection;

const LangsCollection& langsAvailable();

#endif
