#include "ConverterVersion.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

void mappers::ConverterVersion::loadVersion(const std::string& filePath)
{
	registerKeys();
	parseFile(filePath);
	clearRegisteredKeywords();
}

void mappers::ConverterVersion::loadVersion(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::ConverterVersion::registerKeys()
{
	registerSetter("name", name);
	registerSetter("version", version);
	registerSetter("descriptionLine", descriptionLine);
	registerKeyword("minimalIncomingSave", [this](std::istream& theStream) {
		minimalIncomingVersion = GameVersion(commonItems::getString(theStream));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
