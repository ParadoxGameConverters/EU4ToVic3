#include "NameListMapper.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"

void mappers::NameListMapper::loadMappingRules(const std::string& filePath)
{
	Log(LogLevel::Info) << "-> Parsing name list mapping rules.";
	registerKeys();
	parseFile(filePath);
	clearRegisteredKeywords();
	Log(LogLevel::Info) << "<> " << mappings.size() << " rules loaded.";
}

void mappers::NameListMapper::loadMappingRules(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::NameListMapper::registerKeys()
{
	registerKeyword("link", [this](std::istream& theStream) {
		mappings.emplace_back(NameListMapping(theStream));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::optional<mappers::NameListMapping> mappers::NameListMapper::getNamesForCulture(const std::string& eu4Culture, const std::string& eu4CultureGroup) const
{
	for (const auto& mapping: mappings)
		if (!eu4Culture.empty() && mapping.getCulture() == eu4Culture || !eu4CultureGroup.empty() && mapping.getCultureGroup() == eu4CultureGroup)
			return mapping;
	return std::nullopt;
}
