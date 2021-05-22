#include "EU4Technology.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

EU4::EU4Technology::EU4Technology(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::EU4Technology::registerKeys()
{
	registerKeyword("adm_tech", [this](const std::string& unused, std::istream& theStream) {
		adm = commonItems::singleInt(theStream).getInt();
	});
	registerKeyword("dip_tech", [this](const std::string& unused, std::istream& theStream) {
		dip = commonItems::singleInt(theStream).getInt();
	});
	registerKeyword("mil_tech", [this](const std::string& unused, std::istream& theStream) {
		mil = commonItems::singleInt(theStream).getInt();
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
