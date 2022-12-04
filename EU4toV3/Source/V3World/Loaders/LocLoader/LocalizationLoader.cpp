#include "LocalizationLoader.h"
#include "Log.h"
#include <fstream>

void V3::LocalizationLoader::scrapeLocalizations(const commonItems::ModFilesystem& modFS)
{
	Log(LogLevel::Info) << "-> Loading Vic3 localizations.";

	knownLanguages = {"braz_por", "english", "french", "german", "japanese", "korean", "polish", "russian", "simp_chinese", "spanish", "turkish"};

	for (const auto& lang: knownLanguages)
		scrapeLanguage(lang, modFS);

	Log(LogLevel::Info) << ">> " << localizations.size() << " words read.";
}

void V3::LocalizationLoader::scrapeLanguage(const std::string& language, const commonItems::ModFilesystem& modFS)
{
	const auto fileNames = modFS.GetAllFilesInFolderRecursive("localization/" + language + "/");
	for (const auto& file: fileNames)
	{
		std::ifstream fileStream(file);
		if (fileStream.is_open())
			scrapeStream(fileStream, language);
		fileStream.close();
	}
}

void V3::LocalizationLoader::scrapeStream(std::istream& theStream, const std::string& language)
{
	std::string line;
	std::getline(theStream, line); // Dropping first line and BOM with it.

	while (!theStream.eof())
	{
		std::getline(theStream, line);
		const auto [key, value] = determineKeyLocalizationPair(line);
		if (!key.empty() && !value.empty())
		{
			if (localizations.contains(key))
			{
				localizations[key][language] = value;
			}
			else
			{
				LocMap newMap;
				newMap[language] = value;
				localizations.emplace(key, newMap);
			}
		}
	}
}

std::pair<std::string, std::string> V3::LocalizationLoader::determineKeyLocalizationPair(const std::string& text)
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

std::optional<V3::LocMap> V3::LocalizationLoader::getLocMapForKey(const std::string& key) const
{
	const auto& keyItr = localizations.find(key);
	if (keyItr == localizations.end())
		return std::nullopt;
	return keyItr->second;
}

std::optional<std::string> V3::LocalizationLoader::getLocForKey(const std::string& key, const std::string& language) const
{
	const auto& keyItr = localizations.find(key);
	if (keyItr == localizations.end())
		return std::nullopt;

	const auto& locMap = keyItr->second;
	if (locMap.contains(language))
		return locMap.at(language);
	if (locMap.contains("english"))
		return locMap.at("english");

	// If we don't have anything but turkish, we might as well not respond.
	return std::nullopt;
}
