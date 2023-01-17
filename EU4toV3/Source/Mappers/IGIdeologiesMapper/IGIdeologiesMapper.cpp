#include "IGIdeologiesMapper.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"

void mappers::IGIdeologiesMapper::loadMappingRules(const std::string& filePath)
{
	Log(LogLevel::Info) << "-> Loading IG Ideology Mod Rules.";
	registerKeys();
	parseFile(filePath);
	clearRegisteredKeywords();
	Log(LogLevel::Info) << "<> " << mappings.size() << " rules loaded.";
}

void mappers::IGIdeologiesMapper::registerKeys()
{
	registerKeyword("link", [this](std::istream& theStream) {
		auto mapping = IGIdeologiesMapping(theStream);
		mappings.emplace_back(mapping);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::map<std::string, mappers::IGIdeologyMod> mappers::IGIdeologiesMapper::getIGIdeologyMods(const V3::Country& country,
	 const CultureMapper& cultureMapper,
	 const ReligionMapper& religionMapper,
	 const V3::ClayManager& clayManager) const
{
	std::map<std::string, IGIdeologyMod> toReturn;
	for (const auto& mapping: mappings)
		toReturn = mapping.alterIdeologyMods(toReturn, country, cultureMapper, religionMapper, clayManager);
	return toReturn;
}
