#include "ReligionMapper.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"
#include "ReligionMapping.h"

void mappers::ReligionMapper::loadMappingRules(const std::string& filePath)
{
	Log(LogLevel::Info) << "-> Parsing religion mapping rules.";
	registerKeys();
	parseFile(filePath);
	clearRegisteredKeywords();
	Log(LogLevel::Info) << "<> " << eu4ToV3ReligionMap.size() << " rules loaded.";
}

void mappers::ReligionMapper::loadMappingRules(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::ReligionMapper::registerKeys()
{
	registerKeyword("link", [this](std::istream& theStream) {
		const ReligionMapping theMapping(theStream);
		if (!theMapping.getV3Religion().empty() && !theMapping.getEU4Religions().empty())
			for (const auto& eu4Religion: theMapping.getEU4Religions())
				eu4ToV3ReligionMap.emplace(eu4Religion, theMapping.getV3Religion());
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::optional<std::string> mappers::ReligionMapper::getV3Religion(const std::string& eu4Religion) const
{
	if (eu4ToV3ReligionMap.contains(eu4Religion))
		return eu4ToV3ReligionMap.at(eu4Religion);
	return std::nullopt;
}
