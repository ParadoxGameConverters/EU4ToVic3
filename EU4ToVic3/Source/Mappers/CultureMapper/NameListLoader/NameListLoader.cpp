#include "NameListLoader.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"

void mappers::NameListLoader::loadNameLists(const std::string& nameListPath)
{
	Log(LogLevel::Info) << "-> Loading name lists.";
	registerKeys();
	parseFile(nameListPath);
	clearRegisteredKeywords();
	Log(LogLevel::Info) << "<> " << nameLists.size() << " name lists loaded.";
}

void mappers::NameListLoader::loadNameLists(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::NameListLoader::registerKeys()
{
	registerKeyword("country", commonItems::ignoreItem);
	registerKeyword("province", commonItems::ignoreItem);
	registerRegex(commonItems::catchallRegex, [this](const std::string& nameListName, std::istream& theStream) {
		nameLists.emplace(nameListName, NameListEntry(theStream));
	});
}

std::optional<mappers::NameListEntry> mappers::NameListLoader::getNameList(const std::string& nameListName) const
{
	if (nameLists.contains(nameListName))
		return nameLists.at(nameListName);
	return std::nullopt;
}
