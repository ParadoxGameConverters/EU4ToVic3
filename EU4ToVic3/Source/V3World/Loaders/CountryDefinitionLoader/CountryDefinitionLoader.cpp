#include "CountryDefinitionLoader.h"
#include "CommonFunctions.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"
#include "PoliticalManager/Country/Country.h"
#include <ranges>

void V3::CountryDefinitionLoader::loadCommonCountries(const commonItems::ModFilesystem& modFS)
{
	registerKeys();
	for (const auto& fileName: modFS.GetAllFilesInFolder("/common/country_definitions/"))
	{
		if (getExtension(fileName) == "txt")
			parseFile(fileName);
	}
	clearRegisteredKeywords();
}

void V3::CountryDefinitionLoader::reloadCommonCountries(const commonItems::ModFilesystem& vanillaFS)
{
	registerReloadKeys();
	for (const auto& fileName: vanillaFS.GetAllFilesInFolder("/common/country_definitions/"))
	{
		if (getExtension(fileName) == "txt")
			parseFile(fileName);
	}
	clearRegisteredKeywords();
}

void V3::CountryDefinitionLoader::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& countryTag, std::istream& theStream) {
		const auto newCountry = std::make_shared<Country>();
		newCountry->setTag(countryTag);
		newCountry->initializeCountry(theStream);
		if (!newCountry->isCountryDynamic()) // don't use their dynamic tags! Just ignore them.
			countries[countryTag] = newCountry;
	});
}

void V3::CountryDefinitionLoader::registerReloadKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& countryTag, std::istream& theStream) {
		if (countries.contains(countryTag))
		{
			const auto& country = countries.at(countryTag);
			country->storeVanillaCountryType(theStream);
		}
		else
		{
			// one of those dynamic countries we ignore.
			commonItems::ignoreItem("unused", theStream);
		}
	});
}
