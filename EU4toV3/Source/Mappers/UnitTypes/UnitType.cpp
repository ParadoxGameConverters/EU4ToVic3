#include "UnitType.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

mappers::UnitType::UnitType(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();

	// patch for transports
	if (unitType == "transport")
		strength = 24;
}

mappers::UnitType::UnitType(const std::string& filePath)
{
	registerKeys();
	parseFile(filePath);
	clearRegisteredKeywords();

	// patch for transports
	if (unitType == "transport")
		strength = 24;
}

void mappers::UnitType::registerKeys()
{
	registerRegex("maneuver|offensive_morale|defensive_morale|offensive_fire|defensive_fire|offensive_shock|defensive_shock|hull_size",
		 [this](const std::string& unused, std::istream& theStream) {
			 strength += commonItems::singleInt(theStream).getInt();
		 });
	registerKeyword("type", [this](const std::string& unused, std::istream& theStream) {
		unitType = commonItems::singleString(theStream).getString();
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}