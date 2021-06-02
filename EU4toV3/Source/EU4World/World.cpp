#include "World.h"
#include "CommonRegexes.h"
#include "Configuration.h"
#include "EmpireParser/EmpireParser.h"
#include "GameVersion.h"
#include "Log.h"
#include "ModLoader/ModNames.h"
#include "ParserHelpers.h"
#include "rakaly_wrapper.h"
#include <ZipFile.h>
#include <exception>
#include <filesystem>
#include <fstream>
#include <string>
namespace fs = std::filesystem;

EU4::World::World(const Configuration& theConfiguration, const mappers::ConverterVersion& converterVersion)
{
	Log(LogLevel::Info) << "*** Hello EU4, loading World. ***";
	EU4Path = theConfiguration.getEU4Path();
	saveGame.path = theConfiguration.getEU4SaveGamePath();
	Log(LogLevel::Progress) << "6 %";

	Log(LogLevel::Info) << "-> Verifying EU4 save.";
	verifySave();
	Log(LogLevel::Progress) << "7 %";

	Log(LogLevel::Info) << "-> Loading EU4 save.";
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

	Log(LogLevel::Progress) << "-> Verifying Save Contents.";
	verifySaveContents();
	Log(LogLevel::Progress) << "9 %";

	Log(LogLevel::Progress) << "\t* Importing Save. *";
	auto metaData = std::istringstream(saveGame.metadata);
	auto gameState = std::istringstream(saveGame.gamestate);
	registerKeys(theConfiguration, converterVersion);
	parseStream(metaData);
	parseStream(gameState);
	clearRegisteredKeywords();
	Log(LogLevel::Progress) << "\t* Import Complete. *";
	Log(LogLevel::Progress) << "15 %";

	// With mods loaded we can init stuff that requires them.

	Log(LogLevel::Info) << "-> Booting Loaders:";
	Log(LogLevel::Info) << "\tRegions";
	regionManager.loadRegions(EU4Path, mods);
	Log(LogLevel::Info) << "\tReligions";
	religionLoader.loadReligions(EU4Path, mods);
	Log(LogLevel::Info) << "\tCultures";
	cultureLoader.loadCultures(EU4Path, mods);
	Log(LogLevel::Info) << "\tUnit Types";
	countryManager.loadUnitTypes(EU4Path, mods);
	Log(LogLevel::Info) << "\tCommon Countries";
	countryManager.loadCommonCountries(EU4Path, mods);
	Log(LogLevel::Info) << "\tLocalizations";
	countryManager.loadLocalizations(EU4Path, mods);
	Log(LogLevel::Progress) << "16 %";

	Log(LogLevel::Info) << "*** Building world ***";

	Log(LogLevel::Info) << "-> Classifying Provinces According to Aesthetic Principles";
	provinceManager.loadParsers(EU4Path, mods);
	provinceManager.classifyProvinces(regionManager);
	Log(LogLevel::Progress) << "17 %";

	Log(LogLevel::Info) << "-> Calculating Province Weights";
	provinceManager.buildProvinceWeights();
	Log(LogLevel::Progress) << "18 %";

	Log(LogLevel::Info) << "-> Determining Demographics";
	provinceManager.buildPopRatios(datingData, theConfiguration.configBlock.convertAll);
	Log(LogLevel::Progress) << "19 %";

	Log(LogLevel::Info) << "-> Linking Provinces to Countries";
	countryManager.linkProvincesToCountries(provinceManager);
	Log(LogLevel::Progress) << "20 %";

	Log(LogLevel::Info) << "-> Updating Unit Types in Regiments";
	countryManager.updateUnitTypes();
	Log(LogLevel::Progress) << "21 %";

	Log(LogLevel::Info) << "-> Injecting Imperialism into Countries";
	countryManager.setHREAndEmperors(HREmperor, celestialEmperor, provinceManager);
	Log(LogLevel::Progress) << "22 %";

	Log(LogLevel::Info) << "-> Injecting Art Deco into Countries";
	countryManager.injectColorsIntoCountries();
	Log(LogLevel::Progress) << "23 %";

	Log(LogLevel::Info) << "-> Injecting Smokestacks into Provinces";
	countryManager.buildManufactoryCounts();
	Log(LogLevel::Progress) << "24 %";

	Log(LogLevel::Info) << "-> Injecting Localizations into Countries";
	countryManager.injectLocalizations();
	Log(LogLevel::Progress) << "25 %";

	Log(LogLevel::Info) << "-> Viva la revolution?";
	countryManager.setRevolutionTarget(revolutionTarget);
	Log(LogLevel::Progress) << "26 %";

	Log(LogLevel::Info) << "-> Merging Nations";
	countryManager.bootNationMergeMapper();
	countryManager.mergeNations();
	Log(LogLevel::Progress) << "27 %";

	Log(LogLevel::Info) << "-> Equipping Botanical Expedition";
	countryManager.fillHistoricalData();
	Log(LogLevel::Progress) << "38 %";

	Log(LogLevel::Info) << "-> Dropping Dead, Empty and/or Coreless Nations";
	countryManager.filterDeadNations(theConfiguration.configBlock.removeType);
	Log(LogLevel::Progress) << "39 %";

	Log(LogLevel::Info) << "*** Good-bye EU4, you served us well. ***";
	Log(LogLevel::Progress) << "40 %";
}

void EU4::World::registerKeys(const Configuration& theConfiguration, const mappers::ConverterVersion& converterVersion)
{
	registerKeyword("EU4txt", [](std::istream& theStream) {
	});
	registerKeyword("date", [this](std::istream& theStream) {
		datingData.lastEU4Date = date(commonItems::getString(theStream));
	});
	registerKeyword("start_date", [this](std::istream& theStream) {
		datingData.startEU4Date = date(commonItems::getString(theStream));
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
		provinceManager.loadProvinces(theStream);
		Log(LogLevel::Info) << "<> Imported " << provinceManager.getAllProvinces().size() << " provinces.";
	});
	registerKeyword("countries", [this](std::istream& theStream) {
		Log(LogLevel::Info) << "-> Importing Countries";
		countryManager.loadCountries(theStream);
		Log(LogLevel::Info) << "<> Imported " << countryManager.getCountries().size() << " countries.";
	});
	registerKeyword("empire", [this](std::istream& theStream) {
		const EmpireParser empireBlock(theStream);
		HREmperor = empireBlock.getEmperor();
		hreReforms = empireBlock.getHREReforms();
		Log(LogLevel::Info) << "-> HREmperor is: " << HREmperor;
	});
	registerKeyword("celestial_empire", [this](std::istream& theStream) {
		const EmpireParser empireBlock(theStream);
		celestialEmperor = empireBlock.getEmperor();
		Log(LogLevel::Info) << "-> Celestial emperor is: " << celestialEmperor;
	});
	registerKeyword("revolution_target", [this](std::istream& theStream) {
		revolutionTarget = commonItems::getString(theStream);
	});
	registerKeyword("diplomacy", [this](std::istream& theStream) {
		Log(LogLevel::Info) << "-> Loading Diplomacy";
		diplomacyParser.loadDiplomacy(theStream);
		Log(LogLevel::Info) << "-> Loaded " << diplomacyParser.getAgreements().size() << " agreements";
	});
	registerKeyword("active_war", [this](std::istream& theStream) {
		const WarParser newWar(theStream);
		wars.push_back(newWar);
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
