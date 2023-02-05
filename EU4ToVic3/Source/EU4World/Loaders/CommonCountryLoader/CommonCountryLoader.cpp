#include "CommonCountryLoader.h"
#include "CommonFunctions.h"
#include "CommonRegexes.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"
#include <filesystem>
#include <fstream>
#include <ranges>
namespace fs = std::filesystem;

void EU4::CommonCountryLoader::loadCommonCountries(const commonItems::ModFilesystem& modFS)
{
	registerKeys();

	// scoop up all country definitions
	for (const auto& file: modFS.GetAllFilesInFolder("/common/country_tags/"))
	{
		if (getExtension(file) != "txt")
			continue;
		tagParser.parseFile(file);
	}

	// look in each file for color.
	for (const auto& [tag, filePath]: tagPaths)
	{
		auto path = modFS.GetActualFileLocation("/common/" + filePath);
		if (!path)
		{
			Log(LogLevel::Error) << "Where is this file: /common/" + filePath << "?";
			continue;
		}
		color.reset();
		parseFile(*path);
		if (color)
			countryColors.emplace(tag, *color);
	}

	Log(LogLevel::Info) << "<> " << countryColors.size() << " common countries processed.";
	clearRegisteredKeywords();
}

void EU4::CommonCountryLoader::registerKeys()
{
	tagParser.registerRegex(commonItems::catchallRegex, [this](const std::string& tag, std::istream& theStream) {
		tagPaths.emplace(tag, commonItems::getString(theStream));
	});

	registerKeyword("color", [this](std::istream& theStream) {
		color = commonItems::Color::Factory{}.getColor(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::optional<commonItems::Color> EU4::CommonCountryLoader::getCommonColor(const std::string& tag) const
{
	const auto& colorItr = countryColors.find(tag);
	if (colorItr != countryColors.end())
		return colorItr->second;
	else
		return std::nullopt;
}
