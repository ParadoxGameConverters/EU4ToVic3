#include "UnitTypeLoader.h"
#include "CommonFunctions.h"
#include "OSCompatibilityLayer.h"
#include "UnitTypeParser.h"
#include <ranges>

void EU4::UnitTypeLoader::loadUnitTypes(const commonItems::ModFilesystem& modFS)
{
	for (const auto& file: modFS.GetAllFilesInFolder("/common/units/"))
	{
		if (getExtension(file) != "txt")
			continue;
		addUnitFileToRegimentTypeMap(getPath(file), trimPath(file));
	}

	Log(LogLevel::Info) << "<> Loaded " << unitTypeMap.size() << " unit definitions.";
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
