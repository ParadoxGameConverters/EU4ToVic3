#include "SlaveCultureMapper.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "SlaveCultureMapping.h"

void mappers::SlaveCultureMapper::loadMappingRules(const std::filesystem::path& filePath)
{
	Log(LogLevel::Info) << "-> Parsing Slave Culture Mapping Rules.";
	registerKeys();
	parseFile(filePath);
	clearRegisteredKeywords();
	Log(LogLevel::Info) << "<> Loaded " << masterLanguageToSlaveCultureMap.size() + masterHeritageToSlaveCultureMap.size() << " mapping rules.";
}

void mappers::SlaveCultureMapper::registerKeys()
{
	registerKeyword("link", [this](std::istream& theStream) {
		const auto mapping = SlaveCultureMapping(theStream);
		if (mapping.getMasterHeritage())
			masterHeritageToSlaveCultureMap.emplace(*mapping.getMasterHeritage(), mapping.getSlaveCulture());
		if (mapping.getMasterLanguage())
			masterLanguageToSlaveCultureMap.emplace(*mapping.getMasterLanguage(), mapping.getSlaveCulture());
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::optional<std::string> mappers::SlaveCultureMapper::getSlaveCulture(const std::string& masterTrait) const
{
	// Language first, heritage second.
	if (masterLanguageToSlaveCultureMap.contains(masterTrait))
		return masterLanguageToSlaveCultureMap.at(masterTrait);
	if (masterHeritageToSlaveCultureMap.contains(masterTrait))
		return masterHeritageToSlaveCultureMap.at(masterTrait);
	return std::nullopt;
}
