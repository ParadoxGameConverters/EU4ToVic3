#include "EU4Technology.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

EU4::EU4Technology::EU4Technology(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::EU4Technology::registerKeys()
{
	registerSetter("adm_tech", adm);
	registerSetter("dip_tech", dip);
	registerSetter("mil_tech", mil);
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
