#include <map>

#include <QStringList>

#include "StringUtil.h"
#include "JlDtdFileProcessor.h"

#include "test.h"

void test()
{
    JlDtdFileProcessor proc;
    proc.processDtdFile("/home/conductor/_Work/My/SmartTemplate4.Origin/chrome/locale/en-US/smartTemplate-overlay.dtd", "/home/conductor/tmp1", QStringList("ru"));

    //parseDocType("<!ENTITY DevID \"(\\S{1}|\\s{1})\">\n<!ENTITY DevID1 \"(\\S{1}|\\s{1})\">");
}
