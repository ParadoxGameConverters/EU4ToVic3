#include "UnitTypeMapper.h"
#include "CommonFunctions.h"
#include "Configuration.h"
#include "OSCompatibilityLayer.h"
#include <set>

void mappers::UnitTypeMapper::initUnitTypeMapper()
{
	LOG(LogLevel::Info) << "Parsing unit strengths from EU4 installation.";

	for (const auto& filename: commonItems::GetAllFilesInFolder(theConfiguration.getEU4Path() + "/common/units/"))
		addUnitFileToRegimentTypeMap(theConfiguration.getEU4Path() + "/common/units", filename);

	for (const auto& modName: theConfiguration.getEU4Mods())
		for (const auto& filename: commonItems::GetAllFilesInFolder(modName + "/common/units/"))
			addUnitFileToRegimentTypeMap(modName + "/common/units", filename);
}

void mappers::UnitTypeMapper::loadUnitType(const std::string& unitName, std::istream& theStream)
{
	UnitType unitType(theStream);
	unitTypeMap.insert(std::pair(unitName, unitType));
}

void mappers::UnitTypeMapper::addUnitFileToRegimentTypeMap(const std::string& directory, const std::string& filename)
{
	auto name = trimExtension(filename);

	UnitType unitType(directory + "/" + filename);
	if (unitType.getCategory().empty())
	{
		LOG(LogLevel::Warning) << "Unit file for " << name << " at: " << directory << "/" << filename << " has no type!";
		return;
	}

	unitTypeMap.insert(std::pair(name, unitType));
}

std::optional<mappers::UnitType> mappers::UnitTypeMapper::getUnitTypeForRegimentTypeName(const std::string& regimentTypeName) const
{
	if (const auto& typeItr = unitTypeMap.find(regimentTypeName); typeItr != unitTypeMap.end())
		return typeItr->second;
	else
		return std::nullopt;
}
