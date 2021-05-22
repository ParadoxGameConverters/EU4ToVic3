#include "EU4Army.h"
#include "ParserHelpers.h"
#include "UnitTypes/UnitType.h"
#include "UnitTypes/UnitTypeMapper.h"
#include "CommonRegexes.h"

EU4::EU4Army::EU4Army(std::istream& theStream, const std::string& potentialNavy)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();

	if (potentialNavy == "navy")
		armyFloats = true;
}

void EU4::EU4Army::registerKeys()
{
	registerKeyword("name", [this](const std::string& unused, std::istream& theStream) {
		name = commonItems::singleString(theStream).getString();
	});
	registerRegex("regiment|ship", [this](const std::string& unused, std::istream& theStream) {
		const EU4Regiment regimentBlock(theStream);
		regimentList.push_back(regimentBlock);
	});
	registerKeyword("location", [this](const std::string& unused, std::istream& theStream) {
		location = commonItems::singleInt(theStream).getInt();
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

int EU4::EU4Army::getTotalTypeStrength(const std::string& category) const
{
	auto totalStrength = 0;
	for (const auto& regiment: regimentList)
		if (regiment.getCategory() == category)
			totalStrength += regiment.getTypeStrength();

	return totalStrength;
}

void EU4::EU4Army::resolveRegimentTypes(const mappers::UnitTypeMapper& unitTypeMapper)
{
	for (auto& regiment: regimentList)
	{
		const auto& unitType = unitTypeMapper.getUnitTypeForRegimentTypeName(regiment.getType());
		if (unitType)
		{
			regiment.setCategory(unitType->getCategory());
			regiment.setTypeStrength(unitType->getStrength());
		}
		else
		{
			throw std::runtime_error("Illegal unit type: " + regiment.getType() + ", aborting!");
		}
	}
}
