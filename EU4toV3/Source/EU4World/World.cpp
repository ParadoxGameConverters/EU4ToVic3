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

EU4::World::World(const Configuration& theConfiguration, const mappers::ConverterVersion& converterVersion)
{
	Log(LogLevel::Info) << "*** Hello EU4, loading World. ***";
	// Fire up managers.
	datingData = std::make_shared<DatingData>();
	provinceManager = std::make_shared<ProvinceManager>();

	EU4Path = theConfiguration.getEU4Path();
	saveGame.path = theConfiguration.getEU4SaveGamePath();

	Log(LogLevel::Progress) << "6 %";
	Log(LogLevel::Info) << "-> Verifying EU4 save.";
	verifySave();
	Log(LogLevel::Progress) << "7 %";

	Log(LogLevel::Info) << "-> Importing EU4 save.";
	if (!saveGame.compressed)
	{
		std::ifstream inBinary(fs::u8path(saveGame.path), std::ios::binary);
		if (!inBinary.is_open())
		{
			Log(LogLevel::Error) << "Could not open " << saveGame.path << " for parsing.";
			throw std::runtime_error("Could not open " + saveGame.path + " for parsing.");
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
	registerKeys(theConfiguration, converterVersion);
	parseStream(metaData);
	parseStream(gameState);
	clearRegisteredKeywords();
	Log(LogLevel::Progress) << "15 %";

	// With mods loaded we can init stuff that requires them.
	religionLoader.loadReligions(EU4Path, mods);
	cultureLoader.loadCultures(EU4Path, mods);
	Log(LogLevel::Progress) << "16 %";

	Log(LogLevel::Info) << "*** Building world ***";
	Log(LogLevel::Info) << "-> Loading Empires";
	Log(LogLevel::Progress) << "17 %";

	Log(LogLevel::Info) << "-> Calculating Province Weights";
	Log(LogLevel::Progress) << "18 %";

	Log(LogLevel::Info) << "-> Processing Province Info";
	Log(LogLevel::Progress) << "19 %";

	Log(LogLevel::Info) << "-> Loading Regions";
	regionManager.loadRegions(EU4Path);
	Log(LogLevel::Progress) << "21 %";

	Log(LogLevel::Info) << "-> Determining Demographics";
	Log(LogLevel::Progress) << "22 %";

	Log(LogLevel::Info) << "-> Cataloguing Native Fauna";
	Log(LogLevel::Progress) << "24 %";

	Log(LogLevel::Info) << "-> Clasifying Invasive Fauna";
	Log(LogLevel::Progress) << "25 %";

	Log(LogLevel::Info) << "-> Reading Countries";
	Log(LogLevel::Progress) << "26 %";

	Log(LogLevel::Info) << "-> Setting Localizations";
	Log(LogLevel::Progress) << "27 %";

	Log(LogLevel::Info) << "-> Resolving Regiments";
	Log(LogLevel::Progress) << "28 %";

	Log(LogLevel::Info) << "-> Merging Nations";
	Log(LogLevel::Progress) << "29 %";

	Log(LogLevel::Info) << "-> Calculating Industry";
	Log(LogLevel::Progress) << "30 %";

	Log(LogLevel::Info) << "-> Viva la revolution!";
	Log(LogLevel::Progress) << "31 %";

	Log(LogLevel::Info) << "-> Doing Accounting and dishes";
	Log(LogLevel::Progress) << "32 %";

	Log(LogLevel::Info) << "-> Dropping Empty Nations";

	Log(LogLevel::Info) << "*** Good-bye EU4, you served us well. ***";
	Log(LogLevel::Progress) << "40 %";
}

void EU4::World::registerKeys(const Configuration& theConfiguration, const mappers::ConverterVersion& converterVersion)
{
	registerKeyword("EU4txt", [](std::istream& theStream) {
	});
	registerKeyword("date", [this](std::istream& theStream) {
		datingData->lastEU4Date = date(commonItems::getString(theStream));
	});
	registerKeyword("start_date", [this](std::istream& theStream) {
		datingData->startEU4Date = date(commonItems::getString(theStream));
	});
	registerRegex("(multiplayer_)?random_seed", [this](const std::string& unused, std::istream& theStream) {
		auto theSeed = commonItems::getString(theStream);
		if (theSeed.size() > 5)
			theSeed = theSeed.substr(theSeed.size() - 5);
		try
		{
			eu4Seed = std::stoi(theSeed);
		}
		catch (std::exception& e)
		{
			Log(LogLevel::Error) << "Failed reading random_seed, setting 0: " << e.what();
			eu4Seed = 0;
		}
	});
	registerKeyword("savegame_version", [this, converterVersion](std::istream& theStream) {
		version = GameVersion(theStream);
		Log(LogLevel::Info) << "Savegave version: " << version;
		if (version < converterVersion.getMinimalVersion())
			throw std::runtime_error("This converter was built for game version " + converterVersion.getMinimalVersion().toShortString() + "!");
	});
	registerKeyword("mods_enabled_names", [this, theConfiguration](std::istream& theStream) {
		Log(LogLevel::Info) << "-> Detecting used mods.";
		const auto& modBlobs = commonItems::blobList(theStream);
		Log(LogLevel::Info) << "<> Savegame claims " << modBlobs.getBlobs().size() << " mods used:";
		std::map<std::string, std::string> modsList;
		for (const auto& modBlob: modBlobs.getBlobs())
		{
			auto modStream = std::stringstream(modBlob);
			const auto& modName = ModNames(modStream);
			modsList.emplace(modName.getName(), modName.getPath());
			Log(LogLevel::Info) << "---> " << modName.getName() << ": " << modName.getPath();
		}

		// Let's locate, verify and potentially update those mods immediately.
		ModLoader modLoader;
		modLoader.loadMods(theConfiguration, modsList);
		mods = modLoader.getMods();
	});
	registerKeyword("provinces", [this](std::istream& theStream) {
		Log(LogLevel::Info) << "-> Importing Provinces";
		provinceManager->loadProvinces(theStream);
		Log(LogLevel::Info) << "<> Imported " << provinceManager->getAllProvinces().size() << " provinces.";
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

void EU4::World::verifySave()
{
	std::ifstream saveFile(fs::u8path(saveGame.path));
	if (!saveFile.is_open())
		throw std::runtime_error("Could not open save! Exiting!");

	char buffer[3];
	saveFile.get(buffer, 3);
	if (buffer[0] == 'P' && buffer[1] == 'K')
	{
		if (!uncompressSave())
			throw std::runtime_error("Failed to unpack the compressed save!");
	}
	saveFile.close();
}

bool EU4::World::uncompressSave()
{
	auto saveFile = ZipFile::Open(saveGame.path);
	if (!saveFile)
		return false;
	for (size_t entryNum = 0; entryNum < saveFile->GetEntriesCount(); ++entryNum)
	{
		const auto& entry = saveFile->GetEntry(static_cast<int>(entryNum));
		if (const auto& name = entry->GetName(); name == "meta")
		{
			Log(LogLevel::Info) << ">> Uncompressing metadata";
			saveGame.metadata = std::string{std::istreambuf_iterator<char>(*entry->GetDecompressionStream()), std::istreambuf_iterator<char>()};
		}
		else if (name == "gamestate")
		{
			Log(LogLevel::Info) << ">> Uncompressing gamestate";
			saveGame.gamestate = std::string{std::istreambuf_iterator<char>(*entry->GetDecompressionStream()), std::istreambuf_iterator<char>()};
		}
		else if (name == "ai")
		{
			Log(LogLevel::Info) << ">> Uncompressing ai and forgetting it existed";
			saveGame.compressed = true;
		}
		else
			throw std::runtime_error("Unrecognized savegame structure! RNW savegames are NOT supported!");
	}
	return true;
}
