#include "World.h"
#include "CommonFunctions.h"
#include "Configuration.h"
#include "Country/Countries.h"
#include "Country/EU4Country.h"
#include "CultureGroups/CultureGroup.h"
#include "GameVersion.h"
#include "Localization/EU4Localization.h"
#include "Log.h"
#include "Mods/Mods.h"
#include "Mods/ModNames.h"
#include "NationMerger/NationMergeParser.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"
#include "Provinces/EU4Province.h"
#include "Regions/Areas.h"
#include "Regions/SuperRegions.h"
#include "Relations/EU4Empire.h"
#include "StringUtils.h"
#include "rakaly_wrapper.h"
#include <ZipFile.h>
#include <exception>
#include <filesystem>
#include <fstream>
#include <set>
#include <string>
namespace fs = std::filesystem;
#include "CommonRegexes.h"

EU4::World::World(const mappers::IdeaEffectMapper& ideaEffectMapper)
{
	LOG(LogLevel::Info) << "*** Hello EU4, loading World. ***";
	registerKeys(ideaEffectMapper);

	superGroupMapper.init();
	Log(LogLevel::Progress) << "6 %";

	LOG(LogLevel::Info) << "-> Verifying EU4 save.";
	verifySave();
	Log(LogLevel::Progress) << "7 %";

	LOG(LogLevel::Info) << "-> Importing EU4 save.";
	if (!saveGame.compressed)
	{
		std::ifstream inBinary(fs::u8path(theConfiguration.getEU4SaveGamePath()), std::ios::binary);
		if (!inBinary.is_open())
		{
			LOG(LogLevel::Error) << "Could not open " << theConfiguration.getEU4SaveGamePath() << " for parsing.";
			throw std::runtime_error("Could not open " + theConfiguration.getEU4SaveGamePath() + " for parsing.");
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
	cultureGroupsMapper = std::make_shared<mappers::CultureGroups>();
	cultureGroupsMapper->initForEU4();
	buildingTypes = std::make_unique<mappers::Buildings>();
	unitTypeMapper.initUnitTypeMapper();
	Log(LogLevel::Progress) << "16 %";

	LOG(LogLevel::Info) << "*** Building world ***";
	LOG(LogLevel::Info) << "-> Loading Empires";
	setEmpires();
	Log(LogLevel::Progress) << "17 %";

	LOG(LogLevel::Info) << "-> Calculating Province Weights";
	buildProvinceWeights();
	Log(LogLevel::Progress) << "18 %";

	LOG(LogLevel::Info) << "-> Processing Province Info";
	addProvinceInfoToCountries();
	Log(LogLevel::Progress) << "19 %";

	LOG(LogLevel::Info) << "-> Loading Regions";
	loadRegions();
	Log(LogLevel::Progress) << "21 %";

	LOG(LogLevel::Info) << "-> Determining Demographics";
	buildPopRatios();
	Log(LogLevel::Progress) << "22 %";

	LOG(LogLevel::Info) << "-> Cataloguing Native Fauna";
	catalogueNativeCultures();
	Log(LogLevel::Progress) << "24 %";

	LOG(LogLevel::Info) << "-> Clasifying Invasive Fauna";
	generateNeoCultures();
	Log(LogLevel::Progress) << "25 %";

	LOG(LogLevel::Info) << "-> Reading Countries";
	readCommonCountries();
	Log(LogLevel::Progress) << "26 %";

	LOG(LogLevel::Info) << "-> Setting Localizations";
	setLocalizations();
	Log(LogLevel::Progress) << "27 %";

	LOG(LogLevel::Info) << "-> Resolving Regiments";
	resolveRegimentTypes();
	Log(LogLevel::Progress) << "28 %";

	LOG(LogLevel::Info) << "-> Merging Nations";
	mergeNations();
	Log(LogLevel::Progress) << "29 %";

	LOG(LogLevel::Info) << "-> Calculating Industry";
	calculateIndustry();
	Log(LogLevel::Progress) << "30 %";

	LOG(LogLevel::Info) << "-> Viva la revolution!";
	loadRevolutionTarget();
	if (!revolutionTargetString.empty())
	{
		LOG(LogLevel::Info) << " ^^^ Revolution Lives!";
	}
	else
	{
		LOG(LogLevel::Info) << " vvv ... revolution failed. :/";
	}
	Log(LogLevel::Progress) << "31 %";

	LOG(LogLevel::Info) << "-> Doing Accounting and dishes";
	fillHistoricalData();
	Log(LogLevel::Progress) << "32 %";

	LOG(LogLevel::Info) << "-> Dropping Empty Nations";
	removeEmptyNations();
	if (theConfiguration.getRemoveType() == Configuration::DEADCORES::DeadCores)
	{
		removeDeadLandlessNations();
	}
	else if (theConfiguration.getRemoveType() == Configuration::DEADCORES::AllCores)
	{
		removeLandlessNations();
	}
	LOG(LogLevel::Info) << "*** Good-bye EU4, you served us well. ***";
	Log(LogLevel::Progress) << "40 %";
}

void EU4::World::registerKeys(const mappers::IdeaEffectMapper& ideaEffectMapper)
{
	registerKeyword("EU4txt", [](std::istream& theStream) {
	});
	registerKeyword("date", [](std::istream& theStream) {
		theConfiguration.setLastEU4Date(date(commonItems::getString(theStream)));
	});
	registerKeyword("start_date", [](std::istream& theStream) {
		theConfiguration.setStartEU4Date(date(commonItems::getString(theStream)));
	});
	registerRegex("(multiplayer_)?random_seed", [](const std::string& unused, std::istream& theStream) {
		auto theSeed = commonItems::getString(theStream);
		if (theSeed.size() > 5)
			theSeed = theSeed.substr(theSeed.size() - 5);
		try
		{
			theConfiguration.setEU4RandomSeed(std::stoi(theSeed));
		}
		catch (std::exception& e)
		{
			Log(LogLevel::Error) << "Failed reading random_seed, setting 0: " << e.what();
			theConfiguration.setEU4RandomSeed(0);
		}
	});
	registerKeyword("savegame_version", [this](std::istream& theStream) {
		version = std::make_unique<GameVersion>(theStream);
		theConfiguration.setEU4Version(*version);
		Log(LogLevel::Info) << "Savegave version: " << *version;
	});
	registerKeyword("mod_enabled", [](std::istream& theStream) {
		// DEFUNCT since 1.31.
		Log(LogLevel::Info) << "-> Detecting used mods.";
		const auto modsList = commonItems::getStrings(theStream);
		Log(LogLevel::Info) << "<> Savegame claims " << modsList.size() << " mods used:";
		for (const auto& usedMod: modsList)
			Log(LogLevel::Info) << "---> " << usedMod;
		Mods theMods(modsList);
	});
	registerKeyword("mods_enabled_names", [](std::istream& theStream) {
		// In use since 1.31.
		Log(LogLevel::Info) << "-> Detecting used mods.";
		const auto& modBlobs = commonItems::blobList(theStream);
		Log(LogLevel::Info) << "<> Savegame claims " << modBlobs.getBlobs().size() << " mods used:";
		std::vector<std::string> modsList;
		for (const auto& modBlob : modBlobs.getBlobs())
		{
			auto modStream = std::stringstream(modBlob);
			const auto& modName = ModNames(modStream);
			modsList.emplace_back(modName.getPath());
			Log(LogLevel::Info) << "---> " << modName.getName() << ": " << modName.getPath();
		}
		Mods theMods(modsList);
	});
	registerKeyword("revolution_target", [this](std::istream& theStream) {
		revolutionTargetString = commonItems::getString(theStream);
	});
	registerKeyword("celestial_empire", [this](std::istream& theStream) {
		const EU4Empire empireBlock(theStream);
		celestialEmperor = empireBlock.getEmperor();
	});
	registerKeyword("empire", [this](std::istream& theStream) {
		const EU4Empire empireBlock(theStream);
		holyRomanEmperor = empireBlock.getEmperor();
		hreReforms = empireBlock.getHREReforms();
	});
	// Old style of marking the emperor pre-1.20
	registerKeyword("emperor", [this](std::istream& theStream) {
		holyRomanEmperor = commonItems::getString(theStream);
	});
	registerKeyword("provinces", [this](std::istream& theStream) {
		LOG(LogLevel::Info) << "-> Loading Provinces";
		provinces = std::make_unique<Provinces>(theStream);

		const auto& possibleDate = provinces->getProvince(1)->getFirstOwnedDate();
		if (possibleDate)
			theConfiguration.setFirstEU4Date(*possibleDate);
	});
	registerKeyword("countries", [this, ideaEffectMapper](std::istream& theStream) {
		LOG(LogLevel::Info) << "-> Loading Countries";
		const Countries processedCountries(*version, theStream, ideaEffectMapper);
		theCountries = processedCountries.getTheCountries();
	});
	registerKeyword("diplomacy", [this](std::istream& theStream) {
		LOG(LogLevel::Info) << "-> Loading Diplomacy";
		const EU4Diplomacy theDiplomacy(theStream);
		diplomacy = theDiplomacy.getAgreements();
		LOG(LogLevel::Info) << "-> Loaded " << diplomacy.size() << " agreements";
	});
	registerKeyword("active_war", [this](std::istream& theStream) {
		const War newWar(theStream);
		wars.push_back(newWar);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

void EU4::World::calculateIndustry() const
{
	for (const auto& country: theCountries)
		country.second->buildManufactoryCount(theCountries);
}

void EU4::World::buildPopRatios() const
{
	for (const auto& province: provinces->getAllProvinces())
		province.second->buildPopRatio(superGroupMapper, *regions);
}

void EU4::World::generateNeoCultures()
{
	for (const auto& province: provinces->getAllProvinces())
	{
		for (const auto& popRatio: province.second->getPopRatios())
		{
			// Are we operating within native super region for this culture's pop ratio?
			const auto& superRegionName = regions->getParentSuperRegionName(province.first);
			if (!superRegionName)
				continue;
			const auto& currentCulture = popRatio.getCulture();
			if (nativeCultures[*superRegionName].count(currentCulture))
				continue;

			// Are we already a neoculture? Bail if so.
			if (!popRatio.getOriginalCulture().empty())
				continue;

			// Are we within the supergroup? Find out where that culture is native.
			std::string nativeSuperRegionName;
			for (const auto& nativeRegion: nativeCultures)
				if (nativeRegion.second.count(currentCulture))
					nativeSuperRegionName = nativeRegion.first;
			if (nativeSuperRegionName.empty())
			{
				// This is not unusual. Oromo appear later and are not relevant to us.
				// For eu4's american, mexican, brazilian and similar, we'll merge them later with our neocultures through culture_maps.txt.
				continue;
			}
			const auto& currentSuperGroup = superGroupMapper.getGroupForSuperRegion(*superRegionName);
			if (!currentSuperGroup)
			{
				Log(LogLevel::Warning) << "Super-Region " << *superRegionName << " has no defined super-group in worlds_supergroups.txt! Fix this!";
				continue;
			}
			const auto& nativeSuperGroup = superGroupMapper.getGroupForSuperRegion(nativeSuperRegionName);
			if (!nativeSuperGroup)
			{
				Log(LogLevel::Warning) << "Super-Region " << nativeSuperRegionName << " has no defined super-group in worlds_supergroups.txt! Fix this!";
				continue;
			}
			if (*nativeSuperGroup == *currentSuperGroup)
				continue; // Do not mutate within the same super group.

			// Check global cache if we already did this pair.
			std::string neoCulture;
			const auto& genItr = generatedCultures.find(std::make_pair(currentCulture, *superRegionName));
			if (genItr == generatedCultures.end())
			{
				// We need to roll sleeves and get to work.
				neoCulture = generateNeoCulture(*superRegionName, currentCulture);
				generatedCultures.insert(std::make_pair(std::make_pair(currentCulture, *superRegionName), neoCulture));
			}
			else
			{
				neoCulture = genItr->second;
			}
			// Now update the pop ratio.
			province.second->updatePopRatioCulture(currentCulture, neoCulture, *superRegionName);
		}
	}
}

std::string EU4::World::generateNeoCulture(const std::string& superRegionName, const std::string& oldCultureName)
{
	// pull culture group name
	const auto& cultureGroup = cultureGroupsMapper->getGroupForCulture(oldCultureName);
	if (!cultureGroup)
	{
		// Bail gracefully.
		Log(LogLevel::Warning) << "Culture " << oldCultureName << " has no culture group defined! This should not happen!";
		return oldCultureName;
	}

	// This is the new culture name.
	auto neoCultureName = cultureGroup->getName() + "_" + superRegionName + "_culture";

	// Grab culture definitions.
	const auto& cultureItr = cultureGroup->getCultures().find(oldCultureName);
	if (cultureItr == cultureGroup->getCultures().end())
	{
		// what is going in in there?
		Log(LogLevel::Warning) << "Culture " << oldCultureName << " has no culture definitions! This should not happen!";
		return oldCultureName;
	}
	const auto neoCulture = cultureItr->second;

	// We may already have this neoCulture registered (generated by another culture within the same group).
	const auto& neoCultureItr = cultureGroup->getCultures().find(neoCultureName);
	if (neoCultureItr == cultureGroup->getCultures().end())
	{
		// We're golden. Register neoCulture.
		cultureGroup->addNeoCulture(neoCultureName, neoCulture, oldCultureName);
	}
	else
	{
		// We need to append this culture's names on top the existing ones.
		cultureGroup->mergeCulture(neoCultureName, neoCulture);
	}

	return neoCultureName;
}

void EU4::World::catalogueNativeCultures()
{
	for (const auto& province: provinces->getAllProvinces())
	{
		if (province.second->getOriginalCulture().empty())
			continue;
		const auto& superRegionName = regions->getParentSuperRegionName(province.first);
		if (superRegionName)
			nativeCultures[*superRegionName].insert(province.second->getOriginalCulture());
	}
}

void EU4::World::fillHistoricalData()
{
	for (const auto& country: theCountries)
		historicalData.emplace_back(std::make_pair(country.first, country.second->getHistoricalEntry()));
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
	std::ifstream saveFile(fs::u8path(theConfiguration.getEU4SaveGamePath()));
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
	auto saveFile = ZipFile::Open(theConfiguration.getEU4SaveGamePath());
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

void EU4::World::loadRevolutionTarget()
{
	if (!revolutionTargetString.empty())
	{
		auto country = theCountries.find(revolutionTargetString);
		if (country != theCountries.end())
		{
			country->second->viveLaRevolution(true);
		}
	}
}

void EU4::World::buildProvinceWeights() const
{
	for (const auto& province: provinces->getAllProvinces())
		province.second->determineProvinceWeight(*buildingTypes);
}

void EU4::World::addProvinceInfoToCountries()
{
	// add province owner info to countries
	for (const auto& province: provinces->getAllProvinces())
		if (const auto& owner = theCountries.find(province.second->getOwnerString()); owner != theCountries.end())
			owner->second->addProvince(province.second);

	// add province core info to countries
	for (const auto& province: provinces->getAllProvinces())
		for (const auto& core: province.second->getCores())
			if (const auto& country = theCountries.find(core); country != theCountries.end())
				country->second->addCore(province.second);
}

void EU4::World::loadRegions()
{
	if (*version >= GameVersion("1.14"))
		loadEU4RegionsNewVersion();
	else
		loadEU4RegionsOldVersion();
}

void EU4::World::assignProvincesToAreas(const std::map<std::string, std::set<int>>& theAreas) const
{
	for (const auto& [areaName, provinceIDs]: theAreas)
	{
		for (auto provNum: provinceIDs)
		{
			try
			{
				provinces->getProvince(provNum)->setArea(areaName);
			}
			catch (std::exception& e)
			{
				LOG(LogLevel::Warning) << e.what();
			}
		}
	}
}

void EU4::World::loadEU4RegionsOldVersion()
{
	auto regionFilename = theConfiguration.getEU4Path() + "/map/region.txt";

	for (const auto& itr: theConfiguration.getEU4Mods())
	{
		if (!commonItems::DoesFileExist(itr + "/map/region.txt"))
			continue;
		regionFilename = itr + "/map/region.txt";
	}

	std::ifstream theStream(fs::u8path(regionFilename));
	Areas installedAreas(theStream);
	theStream.close();
	assignProvincesToAreas(installedAreas.getAreas());

	regions = std::make_unique<Regions>(installedAreas);
}


void EU4::World::loadEU4RegionsNewVersion()
{
	auto areaFilename = theConfiguration.getEU4Path() + "/map/area.txt";
	auto regionFilename = theConfiguration.getEU4Path() + "/map/region.txt";
	auto superRegionFilename = theConfiguration.getEU4Path() + "/map/superregion.txt";
	for (const auto& itr: theConfiguration.getEU4Mods())
	{
		if (!commonItems::DoesFileExist(itr + "/map/area.txt"))
			continue;
		areaFilename = itr + "/map/area.txt";
	}
	for (const auto& itr: theConfiguration.getEU4Mods())
	{
		if (!commonItems::DoesFileExist(itr + "/map/region.txt"))
			continue;
		regionFilename = itr + "/map/region.txt";
	}
	for (const auto& itr: theConfiguration.getEU4Mods())
	{
		if (!commonItems::DoesFileExist(itr + "/map/superregion.txt"))
			continue;
		superRegionFilename = itr + "/map/superregion.txt";
	}

	std::ifstream areaStream(fs::u8path(areaFilename));
	if (!areaStream.is_open())
		throw std::runtime_error("Could not open map/area.txt!");
	Areas installedAreas(areaStream);
	areaStream.close();
	assignProvincesToAreas(installedAreas.getAreas());

	std::ifstream superRegionFile(fs::u8path(superRegionFilename));
	if (!superRegionFile.is_open())
		throw std::runtime_error("Could not open map/superregion.txt!");
	SuperRegions sRegions(superRegionFile);
	superRegionFile.close();

	std::ifstream regionStream(fs::u8path(regionFilename));
	if (!regionStream.is_open())
		throw std::runtime_error("Could not open map/region.txt!");
	regions = std::make_unique<Regions>(sRegions, installedAreas, regionStream);
	regionStream.close();
}

void EU4::World::readCommonCountries()
{
	std::ifstream commonCountries(fs::u8path(theConfiguration.getEU4Path() + "/common/country_tags/00_countries.txt"));
	if (!commonCountries.is_open())
		throw std::runtime_error("Could not open " + theConfiguration.getEU4Path() + "/common/country_tags/00_countries.txt!");
	readCommonCountriesFile(commonCountries, theConfiguration.getEU4Path());

	for (const auto& itr: theConfiguration.getEU4Mods())
		for (const auto& fileName: commonItems::GetAllFilesInFolder(itr + "/common/country_tags/"))
		{
			std::ifstream convertedCommonCountries(fs::u8path(itr + "/common/country_tags/" + fileName));
			if (!convertedCommonCountries.is_open())
				throw std::runtime_error("Could not open " + itr + "/common/country_tags/" + fileName + "!");
			readCommonCountriesFile(convertedCommonCountries, itr);
		}
}

void EU4::World::readCommonCountriesFile(std::istream& in, const std::string& rootPath)
{
	std::string line;
	while (!in.eof())
	{
		std::getline(in, line);
		auto countryLine = line;
		if (countryLine.size() >= 6 && countryLine[0] != '#' && countryLine[1] != '#')
		{
			// First three characters must be the tag.
			auto tag = countryLine.substr(0, 3);
			if (const auto& countryItr = theCountries.find(tag); countryItr != theCountries.end())
			{
				const auto& country = countryItr->second;

				// Dropping comment after line
				const auto commentPos = countryLine.find('#', 3);
				if (commentPos != std::string::npos)
					countryLine = countryLine.substr(0, commentPos);

				const auto equalPos = countryLine.find('=', 3);
				const auto beginPos = countryLine.find_first_not_of(' ', equalPos + 1);
				const auto endPos = countryLine.find_last_of('\"') + 1;
				auto fileName = countryLine.substr(beginPos, endPos - beginPos);
				fileName = commonItems::remQuotes(fileName); // This will clear quotes if any.

				// Parse the country file.
				auto fullFilename = rootPath + "/common/" + fileName;
				auto localFileName = trimPath(fullFilename);
				if (commonItems::DoesFileExist(fullFilename))
					country->readFromCommonCountry(localFileName, fullFilename);
				else
					Log(LogLevel::Warning) << "Where is country file for " << tag << ": " << fullFilename << "?";
			}
		}
	}
}

void EU4::World::setLocalizations()
{
	const EU4Localization localization;

	for (const auto& theCountry: theCountries)
	{
		const auto& nameLocalizations = localization.getTextInEachLanguage(theCountry.second->getTag());
		if (nameLocalizations)
			for (const auto& [language, name]: *nameLocalizations)
				theCountry.second->setLocalizationName(language, name);

		const auto& adjectiveLocalizations = localization.getTextInEachLanguage(theCountry.second->getTag() + "_ADJ");
		if (adjectiveLocalizations)
			for (const auto& [language, adjective]: *adjectiveLocalizations)
				theCountry.second->setLocalizationAdjective(language, adjective);
	}
}

void EU4::World::resolveRegimentTypes()
{
	for (const auto& country: theCountries)
		country.second->resolveRegimentTypes(unitTypeMapper);
}

void EU4::World::mergeNations()
{
	const NationMergeParser mergeParser;

	if (mergeParser.getMergeDaimyos())
		uniteJapan();

	for (const auto& mergeBlock: mergeParser.getMergeBlocks())
		if (mergeBlock.getMerge() && !mergeBlock.getMaster().empty())
		{
			LOG(LogLevel::Info) << "- Merging nations for: " << mergeBlock.getMaster();
			auto master = getCountry(mergeBlock.getMaster());
			for (const auto& slaveTag: mergeBlock.getSlaves())
			{
				const auto& slave = getCountry(slaveTag);
				master->eatCountry(slave);
			}
		}
}

void EU4::World::uniteJapan()
{
	LOG(LogLevel::Info) << "-> Uniting Japan";

	std::shared_ptr<Country> japan;

	if (*version >= GameVersion("1.20.0.0"))
	{
		for (const auto& country: theCountries)
		{
			if (country.second->getPossibleShogun())
			{
				const auto& tag = country.first;
				LOG(LogLevel::Info) << "- " << tag << " is the shogun.";
				japan = getCountry(tag);
				break;
			}
		}
	}
	else
	{
		japan = getCountry("JAP");
	}
	if (!japan)
		return;
	if (japan->hasFlag("united_daimyos_of_japan"))
		return;

	for (const auto& country: theCountries)
		if (country.second->getPossibleDaimyo())
			japan->eatCountry(country.second);
}

void EU4::World::removeEmptyNations()
{
	std::map<std::string, std::shared_ptr<Country>> survivingCountries;

	for (const auto& country: theCountries)
	{
		const auto& countryProvinces = country.second->getProvinces();
		const auto& countryCores = country.second->getCores();
		if (!countryProvinces.empty() || !countryCores.empty())
			survivingCountries.insert(country);
	}

	theCountries = survivingCountries;
}

void EU4::World::removeDeadLandlessNations()
{
	std::map<std::string, std::shared_ptr<Country>> landlessCountries;
	for (const auto& country: theCountries)
		if (country.second->getProvinces().empty())
			landlessCountries.insert(country);

	for (const auto& country: landlessCountries)
		if (!country.second->cultureSurvivesInCores(theCountries))
			theCountries.erase(country.first);
}

void EU4::World::removeLandlessNations()
{
	std::map<std::string, std::shared_ptr<Country>> survivingCountries;

	for (const auto& country: theCountries)
		if (const auto& theProvinces = country.second->getProvinces(); !theProvinces.empty())
			survivingCountries.insert(country);

	theCountries = survivingCountries;
}

void EU4::World::setEmpires()
{
	for (const auto& country: theCountries)
	{
		// set HRE stuff
		if (country.second->getCapital() != 0 && provinces->getProvince(country.second->getCapital())->inHre())
			country.second->setInHRE(true);
		if (country.second->getTag() == holyRomanEmperor)
			country.second->setEmperor(true);
		if (country.second->getTag() == celestialEmperor)
			country.second->setCelestialEmperor(true);
	}
}

std::shared_ptr<EU4::Country> EU4::World::getCountry(const std::string& tag) const
{
	const auto& country = theCountries.find(tag);
	if (country != theCountries.end())
		return country->second;
	else
		return nullptr;
}
