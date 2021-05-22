#include "TitleMapper.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "TitleMapping.h"
#include <random>
#include "CommonRegexes.h"

mappers::TitleMapper::TitleMapper()
{
	LOG(LogLevel::Info) << "Loading Known Titles.";
	registerKeys();
	parseFile("configurables/title_map.txt");
	clearRegisteredKeywords();
}

mappers::TitleMapper::TitleMapper(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::TitleMapper::registerKeys()
{
	registerKeyword("link", [this](const std::string& unused, std::istream& theStream) {
		const TitleMapping newMapping(theStream);
		if (newMapping.hasIslamicRegion())
			islamicTitles.push_back(newMapping.getTitle());
		else if (newMapping.hasIndianRegion())
			indianTitles.push_back(newMapping.getTitle());
		titleMap[newMapping.getName()] = newMapping.getTitle();
		titles.insert(newMapping.getTitle());
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::optional<std::string> mappers::TitleMapper::getTitleForName(const std::string& name) const
{
	if (const auto& mapping = titleMap.find(name); mapping != titleMap.end())
		return mapping->second;
	else
		return std::nullopt;
}

bool mappers::TitleMapper::doesTitleExist(const std::string& title) const
{
	return titles.contains(title);
}

std::optional<std::string> mappers::TitleMapper::getRandomIslamicTitle() const
{
	if (islamicTitles.empty())
		return std::nullopt;
	std::vector<std::string> randomFlags;
	std::sample(islamicTitles.begin(), islamicTitles.end(), std::inserter(randomFlags, randomFlags.begin()), 1, std::mt19937{std::random_device{}()});
	return *randomFlags.begin();
}

std::optional<std::string> mappers::TitleMapper::getRandomIndianTitle() const
{
	if (indianTitles.empty())
		return std::nullopt;
	std::vector<std::string> randomFlags;
	std::sample(indianTitles.begin(), indianTitles.end(), std::inserter(randomFlags, randomFlags.begin()), 1, std::mt19937{std::random_device{}()});
	return *randomFlags.begin();
}