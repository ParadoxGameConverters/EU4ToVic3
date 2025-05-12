#include "ReligionGroupMapper.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"

void mappers::ReligionGroupMapper::loadMappingRules(const std::string& filePath)
{
	Log(LogLevel::Info) << "-> Parsing religion group mapping rules.";
	registerKeys();
	parseFile(filePath);
	clearRegisteredKeywords();
	Log(LogLevel::Info) << "<> " << mappings.size() << " rules loaded.";
}

void mappers::ReligionGroupMapper::loadMappingRules(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::ReligionGroupMapper::registerKeys()
{
	registerKeyword("link", [this](std::istream& theStream) {
		mappings.emplace_back(ReligionGroupMapping(theStream));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::optional<mappers::ReligionGroupMapping> mappers::ReligionGroupMapper::getMappingForEU4ReligionGroup(const std::string& eu4ReligionGroup) const
{
	for (const auto& mapping: mappings)
		if (mapping.getEU4Groups().contains(eu4ReligionGroup))
			return mapping;
	return std::nullopt;
}
