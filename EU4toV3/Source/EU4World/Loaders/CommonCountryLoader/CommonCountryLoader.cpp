#include "CommonCountryLoader.h"
#include "CommonRegexes.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"
#include <filesystem>
#include <fstream>
#include <ranges>
namespace fs = std::filesystem;

void EU4::CommonCountryLoader::loadCommonCountries(const std::string& EU4Path, const Mods& mods)
{
	registerKeys();

	// scoop up all country definitions
	std::ifstream commonCountries(fs::u8path(EU4Path + "/common/country_tags/00_countries.txt"));
	if (!commonCountries.is_open())
		throw std::runtime_error("Could not open " + EU4Path + "/common/country_tags/00_countries.txt!");
	readCommonCountriesFile(commonCountries, EU4Path);
	commonCountries.close();

	// and mods too.
	for (const auto& itr: mods | std::views::values)
		for (const auto& fileName: commonItems::GetAllFilesInFolder(itr + "/common/country_tags/"))
		{
			std::ifstream convertedCommonCountries(fs::u8path(itr + "/common/country_tags/" + fileName));
			readCommonCountriesFile(convertedCommonCountries, itr);
			convertedCommonCountries.close();
		}

	// look in each file for color.
	for (const auto& [tag, filePath]: tagPaths)
	{
		color.reset();
		parseFile(filePath);
		if (color)
			countryColors.emplace(tag, *color);
	}

	Log(LogLevel::Info) << "<> " << countryColors.size() << " common countries processed.";
	clearRegisteredKeywords();
}

void EU4::CommonCountryLoader::readCommonCountriesFile(std::istream& in, const std::string& rootPath)
{
	while (!in.eof())
	{
		std::string line;
		std::getline(in, line);

		if (line[0] == '#' || line.length() < 4)
			continue;
		auto tag = line.substr(0, 3);

		// All file paths are in quotes. The ones outside are commented, so we can use those as markers.
		auto quoteLoc = line.find_first_of('\"');
		if (quoteLoc == std::string::npos)
		{
			Log(LogLevel::Warning) << "Iffy country line: " << line;
			continue;
		}
		auto countryLine = line.substr(quoteLoc + 1, line.length());
		quoteLoc = countryLine.find_last_of('\"');
		if (quoteLoc == std::string::npos)
		{
			Log(LogLevel::Warning) << "Iffy country line: " << line;
			continue;
		}
		countryLine = countryLine.substr(0, quoteLoc);
		tagPaths.emplace(tag, rootPath + "/common/" + countryLine);
	}
}

void EU4::CommonCountryLoader::registerKeys()
{
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
