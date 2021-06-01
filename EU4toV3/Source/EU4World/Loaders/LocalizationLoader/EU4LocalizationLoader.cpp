#include "EU4LocalizationLoader.h"
#include "OSCompatibilityLayer.h"
#include <filesystem>
#include <fstream>
#include <ranges>
#include <set>
namespace fs = std::filesystem;

void EU4::EU4LocalizationLoader::loadLocalizations(const std::string& EU4Path, const Mods& mods)
{
	readFromAllFilesInFolder(EU4Path + "/localisation");
	for (const auto& itr: mods | std::views::values)
	{
		readFromAllFilesInFolder(itr + "/localisation");
		readFromAllFilesInFolder(itr + "/localisation/replace");
	}
}

void EU4::EU4LocalizationLoader::loadLocalizations(std::istream& theStream)
{
	readFromStream(theStream);
}

void EU4::EU4LocalizationLoader::readFromAllFilesInFolder(const std::string& folderPath)
{
	// There is no need to recurse as EU4 doesn't support subfolders in loc dir except for "replace"
	for (const auto& fileName: commonItems::GetAllFilesInFolder(folderPath))
		readFromFile(folderPath + '/' + fileName);
}

void EU4::EU4LocalizationLoader::readFromFile(const std::string& fileName)
{
	std::ifstream locFile(fs::u8path(fileName));
	readFromStream(locFile);
	locFile.close();
}

void EU4::EU4LocalizationLoader::readFromStream(std::istream& theStream)
{
	std::string line;
	std::getline(theStream, line); // First line is the language is like "l_english:"

	const auto& language = determineLanguageForFile(removeUTF8BOM(line));
	if (!language)
		return;

	while (!theStream.eof())
	{
		std::getline(theStream, line); // Subsequent lines are ' KEY: "Text"'
		const auto [key, value] = determineKeyLocalisationPair(line);
		if (!key.empty() && !value.empty())
			localizations[key][*language] = value;
	}
}

std::optional<std::string> EU4::EU4LocalizationLoader::getText(const std::string& key, const std::string& language) const
{
	const auto keyFindIter = localizations.find(key);
	if (keyFindIter == localizations.end())
		return std::nullopt;

	const auto& localizationsByLanguage = keyFindIter->second;
	const auto& languageFindIter = localizationsByLanguage.find(language);
	if (languageFindIter == localizationsByLanguage.end())
		return std::nullopt;

	return languageFindIter->second;
}

std::optional<std::map<std::string, std::string>> EU4::EU4LocalizationLoader::getTextInEachLanguage(const std::string& key) const
{
	const auto& keyFindIter = localizations.find(key);
	if (keyFindIter == localizations.end())
		return std::nullopt;
	else
		return keyFindIter->second;
}

std::optional<std::string> EU4::EU4LocalizationLoader::determineLanguageForFile(const std::string& text)
{
	if (text.size() < 2 || text[0] != 'l' || text[1] != '_')
		return std::nullopt;

	const size_t beginPos = 2;
	const auto endPos = text.find(':', beginPos);
	if (endPos == std::string::npos)
		return std::nullopt;

	return text.substr(beginPos, endPos - beginPos);
}

std::pair<std::string, std::string> EU4::EU4LocalizationLoader::determineKeyLocalisationPair(const std::string& text)
{
	std::pair<std::string, std::string> blankReturn;

	if (text.size() > 2 && (text[0] == '#' || text[1] == '#'))
		return blankReturn;

	const auto keyBeginPos = text.find_first_not_of(' ');
	if (keyBeginPos == std::string::npos)
		return blankReturn;

	const auto keyEndPos = text.find_first_of(':', keyBeginPos + 1);
	const auto quotePos = text.find_first_of('"', keyEndPos);
	if (quotePos == std::string::npos)
		return blankReturn;

	const auto localizationBeginPos = quotePos + 1;
	const auto localizationEndPos = text.find_last_of('"', text.size());
	return std::make_pair(text.substr(keyBeginPos, keyEndPos - keyBeginPos), text.substr(localizationBeginPos, localizationEndPos - localizationBeginPos));
}

std::string EU4::EU4LocalizationLoader::removeUTF8BOM(const std::string& text)
{
	if (text.size() >= 3 && text[0] == '\xEF' && text[1] == '\xBB' && text[2] == '\xBF')
		return text.substr(3);
	else
		return text;
}
