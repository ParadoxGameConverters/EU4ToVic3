#include "EU4Army.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "UnitTypeLoader/UnitTypeLoader.h"

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
	registerSetter("name", name);
	registerSetter("location", location);
	registerRegex("regiment|ship", [this](const std::string& unused, std::istream& theStream) {
		const EU4Regiment regimentBlock(theStream);
		regimentList.push_back(regimentBlock);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

int EU4::EU4Army::getTotalTypeStrength(const std::string& type) const
{
	auto totalStrength = 0;
	for (const auto& regiment: regimentList)
		if (regiment.getType() == type)
			totalStrength += regiment.getStrength();

	return totalStrength;
}

void EU4::EU4Army::updateRegimentTypes(const UnitTypeLoader& unitTypeLoader)
{
	for (auto& regiment: regimentList)
	{
		const auto& unitType = unitTypeLoader.getUnitTypeForRegimentTypeName(regiment.getType());
		if (unitType)
		{
			regiment.updateUnitType(*unitType); // this replaces "western_medieval_knights" with "infantry" of strength "5".
		}
		else
		{
			Log(LogLevel::Warning) << "Illegal unit type: " + regiment.getType() + ", your EU4 installation doesn't have this unit!";

			// Well then. Let's play silly.
			UnitType sillyUnitType;
			sillyUnitType.strength = 1;
			if (!armyFloats)
				sillyUnitType.unitType = "infantry";
			else
				sillyUnitType.unitType = "transport";
			regiment.updateUnitType(sillyUnitType);
		}
	}
}
