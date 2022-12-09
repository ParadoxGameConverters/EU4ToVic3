#include "CultureTraitMapper.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"

void mappers::CultureTraitMapper::loadMappingRules(const std::string& filePath)
{
	Log(LogLevel::Info) << "-> Parsing culture trait mapping rules.";
	registerKeys();
	parseFile(filePath);
	clearRegisteredKeywords();
	Log(LogLevel::Info) << "<> " << mappings.size() << " rules loaded.";
}

void mappers::CultureTraitMapper::loadMappingRules(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::CultureTraitMapper::registerKeys()
{
	registerKeyword("link", [this](std::istream& theStream) {
		mappings.emplace_back(CultureTraitMapping(theStream));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::optional<mappers::CultureTraitMapping> mappers::CultureTraitMapper::getTraitsForCulture(const std::string& eu4Culture,
	 const std::string& eu4CultureGroup) const
{
	for (const auto& mapping: mappings)
		if (!eu4Culture.empty() && mapping.getCulture() == eu4Culture || !eu4CultureGroup.empty() && mapping.getCultureGroup() == eu4CultureGroup)
			return mapping;
	return std::nullopt;
}
