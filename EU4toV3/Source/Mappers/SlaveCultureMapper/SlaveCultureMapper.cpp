#include "SlaveCultureMapper.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "SlaveCultureMapping.h"

void mappers::SlaveCultureMapper::loadMappingRules(const std::string& filePath)
{
	Log(LogLevel::Info) << "-> Parsing Slave Culture Mapping Rules.";
	registerKeys();
	parseFile(filePath);
	clearRegisteredKeywords();
	Log(LogLevel::Info) << "<> Loaded " << masterTraitToSlaveCultureMap.size() << " mapping rules.";
}

void mappers::SlaveCultureMapper::registerKeys()
{
	registerKeyword("link", [this](std::istream& theStream) {
		const auto mapping = SlaveCultureMapping(theStream);
		masterTraitToSlaveCultureMap.emplace(mapping.getMasterTrait(), mapping.getSlaveCulture());
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::optional<std::string> mappers::SlaveCultureMapper::getSlaveCulture(const std::string& masterTrait) const
{
	if (masterTraitToSlaveCultureMap.contains(masterTrait))
		return masterTraitToSlaveCultureMap.at(masterTrait);
	return std::nullopt;
}
