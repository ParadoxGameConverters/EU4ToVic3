#include "VNColonialMapper.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

void mappers::VNColonialMapper::loadMappingRules(const std::string& fileName)
{
	registerKeys();
	parseFile(fileName);
	clearRegisteredKeywords();
}

void mappers::VNColonialMapper::loadMappingRules(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::VNColonialMapper::registerKeys()
{
	registerKeyword("link", [this](std::istream& theStream) {
		VNColonialMapping theMapping(theStream);
		VNColonies.emplace_back(theMapping);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

bool mappers::VNColonialMapper::isStateVNColonial(const std::string& stateName) const
{
	for (const auto& colony: VNColonies)
		if (colony.getStates().contains(stateName))
			return true;
	return false;
}

std::set<std::string> mappers::VNColonialMapper::getVanillaOwners(const std::string& stateName) const
{
	std::set<std::string> owners;
	for (const auto& colony: VNColonies)
		if (colony.getStates().contains(stateName))
			owners.emplace(colony.getOwner());
	return owners;
}

std::optional<std::string> mappers::VNColonialMapper::getKeyProvince(const std::string& stateName, const std::string& ownerTag) const
{
	for (const auto& colony: VNColonies)
		if (colony.getStates().contains(stateName) && colony.getOwner() == ownerTag)
			return colony.getKeyProvince();
	return std::nullopt;
}

bool mappers::VNColonialMapper::isStateDecolonizable(const std::string& stateName, const std::string& ownerTag, const std::string& currentOwner) const
{
	for (const auto& colony: VNColonies)
	{
		if (colony.getDecolonizeBlocker().empty())
			continue;
		if (colony.getStates().contains(stateName) && colony.getOwner() == ownerTag && colony.getDecolonizeBlocker() != currentOwner)
			return true;
	}
	return false;
}
