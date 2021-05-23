#include "World.h"
#include "Configuration.h"
#include "GameVersion.h"
#include "Log.h"
#include "Mods/ModNames.h"
#include "ParserHelpers.h"
#include "rakaly_wrapper.h"
#include <ZipFile.h>
#include <exception>
#include <filesystem>
#include <fstream>
#include <string>
namespace fs = std::filesystem;
#include "CommonRegexes.h"

EU4::World::World(const std::shared_ptr<Configuration>& theConfiguration)
{
	LOG(LogLevel::Info) << "*** Hello EU4, loading World. ***";
	registerKeys(theConfiguration);

	Log(LogLevel::Progress) << "6 %";

	LOG(LogLevel::Info) << "-> Verifying EU4 save.";
	verifySave(theConfiguration->getEU4SaveGamePath());
	Log(LogLevel::Progress) << "7 %";

	LOG(LogLevel::Info) << "-> Importing EU4 save.";
	if (!saveGame.compressed)
	{
		std::ifstream inBinary(fs::u8path(theConfiguration->getEU4SaveGamePath()), std::ios::binary);
		if (!inBinary.is_open())
		{
			LOG(LogLevel::Error) << "Could not open " << theConfiguration->getEU4SaveGamePath() << " for parsing.";
			throw std::runtime_error("Could not open " + theConfiguration->getEU4SaveGamePath() + " for parsing.");
		}
		std::stringstream inStream;
		inStream << inBinary.rdbuf();
		saveGame.gamestate = inStream.str();
	}
	Log(LogLevel::Progress) << "8 %";

	verifySaveContents();
	Log(LogLevel::Progress) << "9 %";

	auto metaData = std::istringstream(saveGame.metadata);
	auto gameState = std::istringstream(saveGame.gamestate);
	parseStream(metaData);
	parseStream(gameState);
	Log(LogLevel::Progress) << "15 %";

	clearRegisteredKeywords();
	// With mods loaded we can init stuff that requires them.
	
	Log(LogLevel::Progress) << "16 %";

	LOG(LogLevel::Info) << "*** Building world ***";
	LOG(LogLevel::Info) << "-> Loading Empires";
	Log(LogLevel::Progress) << "17 %";

	LOG(LogLevel::Info) << "-> Calculating Province Weights";
	Log(LogLevel::Progress) << "18 %";

	LOG(LogLevel::Info) << "-> Processing Province Info";
	Log(LogLevel::Progress) << "19 %";

	LOG(LogLevel::Info) << "-> Loading Regions";
	Log(LogLevel::Progress) << "21 %";

	LOG(LogLevel::Info) << "-> Determining Demographics";
	Log(LogLevel::Progress) << "22 %";

	LOG(LogLevel::Info) << "-> Cataloguing Native Fauna";
	Log(LogLevel::Progress) << "24 %";

	LOG(LogLevel::Info) << "-> Clasifying Invasive Fauna";
	Log(LogLevel::Progress) << "25 %";

	LOG(LogLevel::Info) << "-> Reading Countries";
	Log(LogLevel::Progress) << "26 %";

	LOG(LogLevel::Info) << "-> Setting Localizations";
	Log(LogLevel::Progress) << "27 %";

	LOG(LogLevel::Info) << "-> Resolving Regiments";
	Log(LogLevel::Progress) << "28 %";

	LOG(LogLevel::Info) << "-> Merging Nations";
	Log(LogLevel::Progress) << "29 %";

	LOG(LogLevel::Info) << "-> Calculating Industry";
	Log(LogLevel::Progress) << "30 %";

	LOG(LogLevel::Info) << "-> Viva la revolution!";
	Log(LogLevel::Progress) << "31 %";

	LOG(LogLevel::Info) << "-> Doing Accounting and dishes";
	Log(LogLevel::Progress) << "32 %";

	LOG(LogLevel::Info) << "-> Dropping Empty Nations";
	
	LOG(LogLevel::Info) << "*** Good-bye EU4, you served us well. ***";
	Log(LogLevel::Progress) << "40 %";
}

void EU4::World::registerKeys(const std::shared_ptr<Configuration>& theConfiguration)
{
	registerKeyword("EU4txt", [](std::istream& theStream) {
	});
	registerKeyword("date", [theConfiguration](std::istream& theStream) {
		theConfiguration->setLastEU4Date(date(commonItems::getString(theStream)));
	});
	registerKeyword("start_date", [theConfiguration](std::istream& theStream) {
		theConfiguration->setStartEU4Date(date(commonItems::getString(theStream)));
	});
	registerRegex("(multiplayer_)?random_seed", [theConfiguration](const std::string& unused, std::istream& theStream) {
		auto theSeed = commonItems::getString(theStream);
		if (theSeed.size() > 5)
			theSeed = theSeed.substr(theSeed.size() - 5);
		try
		{
			theConfiguration->setEU4RandomSeed(std::stoi(theSeed));
		}
		catch (std::exception& e)
		{
			Log(LogLevel::Error) << "Failed reading random_seed, setting 0: " << e.what();
			theConfiguration->setEU4RandomSeed(0);
		}
	});
	registerKeyword("savegame_version", [this, theConfiguration](std::istream& theStream) {
		version = std::make_unique<GameVersion>(theStream);
		theConfiguration->setEU4Version(*version);
		Log(LogLevel::Info) << "Savegave version: " << *version;
	});
	registerKeyword("mods_enabled_names", [theConfiguration](std::istream& theStream) {
		// In use since 1.31.
		Log(LogLevel::Info) << "-> Detecting used mods.";
		const auto& modBlobs = commonItems::blobList(theStream);
		Log(LogLevel::Info) << "<> Savegame claims " << modBlobs.getBlobs().size() << " mods used:";
		std::map<std::string, std::string> modsList;
		for (const auto& modBlob : modBlobs.getBlobs())
		{
			auto modStream = std::stringstream(modBlob);
			const auto& modName = ModNames(modStream);
			modsList.emplace(modName.getName(), modName.getPath());
			Log(LogLevel::Info) << "---> " << modName.getName() << ": " << modName.getPath();
		}
		theConfiguration->setEU4Mods(modsList);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}


void EU4::World::verifySaveContents()
{
	if (saveGame.gamestate.starts_with("EU4bin"))
	{
		saveGame.gamestate = rakaly::meltEU4(saveGame.gamestate);
		saveGame.metadata = rakaly::meltEU4(saveGame.metadata);
	}
}

void EU4::World::verifySave(const std::string& saveGamePath)
{
	std::ifstream saveFile(fs::u8path(saveGamePath));
	if (!saveFile.is_open())
		throw std::runtime_error("Could not open save! Exiting!");

	char buffer[3];
	saveFile.get(buffer, 3);
	if (buffer[0] == 'P' && buffer[1] == 'K')
	{
		if (!uncompressSave(saveGamePath))
			throw std::runtime_error("Failed to unpack the compressed save!");
	}
	saveFile.close();
}

bool EU4::World::uncompressSave(const std::string& saveGamePath)
{
	auto saveFile = ZipFile::Open(saveGamePath);
	if (!saveFile)
		return false;
	for (size_t entryNum = 0; entryNum < saveFile->GetEntriesCount(); ++entryNum)
	{
		const auto& entry = saveFile->GetEntry(static_cast<int>(entryNum));
		const auto& name = entry->GetName();
		if (name == "meta")
		{
			LOG(LogLevel::Info) << ">> Uncompressing metadata";
			saveGame.metadata = std::string{std::istreambuf_iterator<char>(*entry->GetDecompressionStream()), std::istreambuf_iterator<char>()};
		}
		else if (name == "gamestate")
		{
			LOG(LogLevel::Info) << ">> Uncompressing gamestate";
			saveGame.gamestate = std::string{std::istreambuf_iterator<char>(*entry->GetDecompressionStream()), std::istreambuf_iterator<char>()};
		}
		else if (name == "ai")
		{
			LOG(LogLevel::Info) << ">> Uncompressing ai and forgetting it existed";
			saveGame.compressed = true;
		}
		else
			throw std::runtime_error("Unrecognized savegame structure!");
	}
	return true;
}
