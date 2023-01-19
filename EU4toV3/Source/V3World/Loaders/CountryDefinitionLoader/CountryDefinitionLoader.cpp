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

void V3::CountryDefinitionLoader::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& countryTag, std::istream& theStream) {
		const auto newCountry = std::make_shared<Country>();
		newCountry->setTag(countryTag);
		newCountry->initializeCountry(theStream);
		countries.emplace(countryTag, newCountry);
	});
}
