#include "NewEU4CultureMapper.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"

void mappers::NewEU4CultureMapper::loadMappingRules(const std::string& filePath)
{
	Log(LogLevel::Info) << "-> Parsing *_new EU4 culture mapping rules.";
	registerKeys();
	parseFile(filePath);
	clearRegisteredKeywords();
	Log(LogLevel::Info) << "<> " << mappings.size() << " rules loaded.";
}

void mappers::NewEU4CultureMapper::loadMappingRules(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::NewEU4CultureMapper::registerKeys()
{
	registerKeyword("link", [this](std::istream& theStream) {
		auto mapping = NewEU4CultureMapping(theStream);

		for (const auto& culture: mapping.getCultures())
			interestingCultures.emplace(culture);

		mappings.emplace_back(mapping);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::set<std::string> mappers::NewEU4CultureMapper::getAddTraitsForCulture(const std::string& eu4Culture) const
{
	if (eu4Culture.empty())
		return std::set<std::string>{};

	for (const auto& mapping: mappings)
		if (mapping.getCultures().contains(eu4Culture))
			return mapping.getAddTraits();

	return std::set<std::string>{};
}

std::set<std::string> mappers::NewEU4CultureMapper::getRemoveTraitsForCulture(const std::string& eu4Culture) const
{
	if (eu4Culture.empty())
		return std::set<std::string>{};

	for (const auto& mapping: mappings)
		if (mapping.getCultures().contains(eu4Culture))
			return mapping.getRemoveTraits();

	return std::set<std::string>{};
}
