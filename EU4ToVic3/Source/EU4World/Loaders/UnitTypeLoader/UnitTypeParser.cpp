#include "UnitTypeParser.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

EU4::UnitTypeParser::UnitTypeParser(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

EU4::UnitTypeParser::UnitTypeParser(const std::string& filePath)
{
	registerKeys();
	parseFile(filePath);
	clearRegisteredKeywords();
}

void EU4::UnitTypeParser::registerKeys()
{
	registerKeyword("type", [this](std::istream& theStream) {
		unitType.unitType = commonItems::getString(theStream);
	});
	registerRegex("maneuver|offensive_morale|defensive_morale|offensive_fire|defensive_fire|offensive_shock|defensive_shock|hull_size",
		 [this](const std::string& unused, std::istream& theStream) {
			 unitType.strength += commonItems::singleInt(theStream).getInt();
		 });
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}