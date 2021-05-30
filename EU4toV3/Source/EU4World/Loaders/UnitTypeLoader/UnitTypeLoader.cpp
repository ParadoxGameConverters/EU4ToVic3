#include "UnitTypeLoader.h"
#include "CommonFunctions.h"
#include "OSCompatibilityLayer.h"
#include "UnitTypeParser.h"
#include <ranges>

void EU4::UnitTypeLoader::loadUnitTypes(const std::string& EU4Path, const EU4::Mods& mods)
{
	Log(LogLevel::Info) << "-> Loading unit types and strengths.";

	for (const auto& filename: commonItems::GetAllFilesInFolder(EU4Path + "/common/units/"))
		addUnitFileToRegimentTypeMap(EU4Path + "/common/units", filename);

	for (const auto& modPath: mods | std::views::values)
		for (const auto& filename: commonItems::GetAllFilesInFolder(modPath + "/common/units/"))
			addUnitFileToRegimentTypeMap(modPath + "/common/units", filename);
}

void EU4::UnitTypeLoader::loadUnitType(const std::string& unitName, std::istream& theStream)
{
	const UnitTypeParser unitType(theStream);
	unitTypeMap.emplace(unitName, unitType.getUnitType());
}

void EU4::UnitTypeLoader::addUnitFileToRegimentTypeMap(const std::string& directory, const std::string& filename)
{
	auto name = trimExtension(filename);

	const UnitTypeParser unitType(directory + "/" + filename);
	if (unitType.getUnitType().unitType.empty())
	{
		Log(LogLevel::Warning) << "Unit file for " << name << " at: " << directory << "/" << filename << " has no type!";
		return;
	}

	unitTypeMap.emplace(name, unitType.getUnitType());
}

std::optional<EU4::UnitType> EU4::UnitTypeLoader::getUnitTypeForRegimentTypeName(const std::string& regimentTypeName) const
{
	if (const auto& typeItr = unitTypeMap.find(regimentTypeName); typeItr != unitTypeMap.end())
		return typeItr->second;
	else
		return std::nullopt;
}
