#include "RegionLocalizations.h"
#include "../../Configuration.h"
#include "OSCompatibilityLayer.h"
#include <fstream>
#include <filesystem>
#include "Log.h"
namespace fs = std::filesystem;

mappers::RegionLocalizations::RegionLocalizations()
{
	LOG(LogLevel::Info) << "Parsing Region Localizations.";

	auto locFiles = commonItems::GetAllFilesInFolder(theConfiguration.getEU4Path() + "/localisation/");
	for (const auto& locFile : locFiles)
	{
		auto position = locFile.find("l_english");
		if (position != std::string::npos)
		{
			auto locMap = chopFile(locFile);
			engLocalisations.insert(locMap.begin(), locMap.end());
		}		
		position = locFile.find("l_french");
		if (position != std::string::npos)
		{
			auto locMap = chopFile(locFile);
			fraLocalisations.insert(locMap.begin(), locMap.end());
		}
		position = locFile.find("l_spanish");
		if (position != std::string::npos)
		{
			auto locMap = chopFile(locFile);
			spaLocalisations.insert(locMap.begin(), locMap.end());
		}
		position = locFile.find("l_german");
		if (position != std::string::npos)
		{
			auto locMap = chopFile(locFile);
			gerLocalisations.insert(locMap.begin(), locMap.end());
		}
	}
	LOG(LogLevel::Info) << "Loaded: " << engLocalisations.size() << " " << fraLocalisations.size() << " " << spaLocalisations.size() << " " << gerLocalisations.size();

}

std::map<std::string, std::string> mappers::RegionLocalizations::chopFile(const std::string& locFile) const
{
	auto theFile = std::ifstream(fs::u8path(theConfiguration.getEU4Path() + "/localisation/" + locFile));
	if (!theFile.is_open()) throw std::runtime_error("Cannot open area/region localizations at " + theConfiguration.getEU4Path() + "/localisation/" + locFile);
	
	std::map<std::string, std::string> storedLocs;
	std::string line;
	// drop first line
	std::getline(theFile, line);
	
	while (std::getline(theFile, line))
	{
		const auto position = line.find_first_of(":");
		if (position == std::string::npos) continue;

		auto key = line.substr(1, position - 1);
		if (line.length() < position + 3) continue;
		auto loc = line.substr(position + 2, line.length());
		if (loc.length() < 5) continue;
		loc = loc.substr(2, loc.length() - 3);
		storedLocs.insert(std::make_pair(key, loc));
	}
	theFile.close();
	return storedLocs;
}

std::optional<std::string> mappers::RegionLocalizations::getEnglishFor(const std::string& key)
{
	auto itr = engLocalisations.find(key);
	if (itr != engLocalisations.end()) return itr->second;
	return std::nullopt;
}

std::optional<std::string> mappers::RegionLocalizations::getFrenchFor(const std::string& key)
{
	auto itr = fraLocalisations.find(key);
	if (itr != fraLocalisations.end()) return itr->second;
	return std::nullopt;
}

std::optional<std::string> mappers::RegionLocalizations::getSpanishFor(const std::string& key)
{
	auto itr = spaLocalisations.find(key);
	if (itr != spaLocalisations.end()) return itr->second;
	return std::nullopt;
}

std::optional<std::string> mappers::RegionLocalizations::getGermanFor(const std::string& key)
{
	auto itr = gerLocalisations.find(key);
	if (itr != gerLocalisations.end()) return itr->second;
	return std::nullopt;
}
