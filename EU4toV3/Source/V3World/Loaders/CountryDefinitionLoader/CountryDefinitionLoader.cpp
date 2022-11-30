#include "CountryDefinitionLoader.h"
#include "CommonFunctions.h"
#include "CommonRegexes.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"
#include "PoliticalManager/Country/Country.h"
#include <ranges>

void V3::CountryDefinitionLoader::loadCommonCountries(const std::string& v3Path)
{
	registerKeys();
	for (const auto& fileName: commonItems::GetAllFilesInFolder(v3Path + "/common/country_definitions/"))
	{
		if (getExtension(fileName) == "txt")
			parseFile(v3Path + "/common/country_definitions/" + fileName);
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
