#include "CultureTraitMapper.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"

void mappers::CultureTraitMapper::loadMappingRules(const std::filesystem::path& heritageFilePath,
	 const std::filesystem::path& languageFilePath,
	 const std::filesystem::path& traditionFilePath)
{
	Log(LogLevel::Info) << "-> Parsing culture trait mapping rules.";
	registerHeritageKeys();
	parseFile(heritageFilePath);
	clearRegisteredKeywords();
	registerLanguageKeys();
	parseFile(languageFilePath);
	clearRegisteredKeywords();
	registerTraditionKeys();
	parseFile(traditionFilePath);
	clearRegisteredKeywords();
	Log(LogLevel::Info) << "<> " << heritageMappings.size() + languageMappings.size() + traditionMappings.size() << " rules loaded.";
}

void mappers::CultureTraitMapper::loadMappingRules(std::istream& heritageStream, std::istream& languageStream, std::istream& traditionStream)
{
	registerHeritageKeys();
	parseStream(heritageStream);
	clearRegisteredKeywords();
	registerLanguageKeys();
	parseStream(languageStream);
	clearRegisteredKeywords();
	registerTraditionKeys();
	parseStream(traditionStream);
	clearRegisteredKeywords();
}

void mappers::CultureTraitMapper::registerHeritageKeys()
{
	registerKeyword("link", [this](std::istream& theStream) {
		heritageMappings.emplace_back(CultureTraitMapping(theStream));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

void mappers::CultureTraitMapper::registerLanguageKeys()
{
	registerKeyword("link", [this](std::istream& theStream) {
		languageMappings.emplace_back(CultureTraitMapping(theStream));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

void mappers::CultureTraitMapper::registerTraditionKeys()
{
	registerKeyword("link", [this](std::istream& theStream) {
		traditionMappings.emplace_back(CultureTraitMapping(theStream));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::optional<mappers::CultureTraitMapping> mappers::CultureTraitMapper::getTraitsForCulture(const std::string& eu4Culture,
	 const std::string& eu4CultureGroup) const
{
	CultureTraitMapping toReturn;
	bool match = false;
	for (const auto& mapping: heritageMappings)
	{
		if (!eu4Culture.empty() && mapping.getCultures().contains(eu4Culture))
		{
			match = true;
			toReturn = mapping;
		}
	}
	if (!match)
	{
		for (const auto& mapping: heritageMappings)
		{
			if (!eu4CultureGroup.empty() && mapping.getCultureGroups().contains(eu4CultureGroup))
			{
				match = true;
				toReturn = mapping;
			}
		}
	}

	if (!match)
	{
		return std::nullopt; // We found nothing, bail.
	}

	// We found something, let's fill up the language and traditions (if any).

	match = false;
	for (const auto& mapping: languageMappings)
	{
		if (!eu4Culture.empty() && mapping.getCultures().contains(eu4Culture))
		{
			match = true;
			toReturn.setLanguage(mapping.getLanguage());
		}
	}
	if (!match)
	{
		for (const auto& mapping: languageMappings)
		{
			if (!eu4CultureGroup.empty() && mapping.getCultureGroups().contains(eu4CultureGroup))
			{
				match = true;
				toReturn.setLanguage(mapping.getLanguage());
			}
		}
	}
	if (!match)
	{
		Log(LogLevel::Warning) << "EU4 Culture " << eu4Culture << " from " << eu4CultureGroup
									  << " has no language mapped in culture_trait_language_map.txt! This is severely bad. Fix this asap! Assigning ... GREEK.";
		toReturn.setLanguage("language_greek");
	}

	match = false;
	for (const auto& mapping: traditionMappings)
	{
		if (!eu4Culture.empty() && mapping.getCultures().contains(eu4Culture))
		{
			match = true;
			toReturn.setTraditions(mapping.getTraditions());
		}
	}
	if (!match)
	{
		for (const auto& mapping: traditionMappings)
		{
			if (!eu4CultureGroup.empty() && mapping.getCultureGroups().contains(eu4CultureGroup))
			{
				toReturn.setTraditions(mapping.getTraditions());
			}
		}
	}
	return toReturn;
}
