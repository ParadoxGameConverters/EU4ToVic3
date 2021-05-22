#include "VersionParser.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

mappers::VersionParser::VersionParser()
{
	registerKeys();
	parseFile("configurables/version.txt");
	clearRegisteredKeywords();
}

mappers::VersionParser::VersionParser(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::VersionParser::registerKeys()
{
	registerKeyword("name", [this](const std::string& unused, std::istream& theStream) {
		name = commonItems::getString(theStream);
	});
	registerKeyword("version", [this](const std::string& unused, std::istream& theStream) {
		version = commonItems::getString(theStream);
	});
	registerKeyword("descriptionLine", [this](const std::string& unused, std::istream& theStream) {
		descriptionLine = commonItems::getString(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
