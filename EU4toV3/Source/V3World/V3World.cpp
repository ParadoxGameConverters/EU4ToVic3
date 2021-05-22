#include "V3World.h"
#include "../EU4World/World.h"
#include "../Helpers/TechValues.h"
#include "../Mappers/Pops/PopMapper.h"
#include "../Mappers/TechGroups/TechGroupsMapper.h"
#include "../Mappers/VersionParser/VersionParser.h"
#include "CommonFunctions.h"
#include "Configuration.h"
#include "CultureGroups/CultureGroup.h"
#include "Flags/Flags.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include <cfloat>
#include <filesystem>
#include <fstream>
#include <queue>
namespace fs = std::filesystem;

constexpr int MAX_EQUALITY_COUNTRIES = 5;
constexpr int MAX_LIBERTY_COUNTRIES = 20;

V2::World::World(const EU4::World& sourceWorld,
	 const mappers::IdeaEffectMapper& ideaEffectMapper,
	 const mappers::TechGroupsMapper& techGroupsMapper,
	 const mappers::VersionParser& versionParser):
	 historicalData(sourceWorld.getHistoricalData())
{
	Log(LogLevel::Progress) << "45 %";

	LOG(LogLevel::Info) << "Parsing cultural union mappings.";
	culturalUnionMapper = std::make_unique<mappers::CulturalUnionMapper>("configurables/unions.txt");
	LOG(LogLevel::Info) << "Parsing nationalities mappings.";
	culturalNationalitiesMapper = std::make_unique<mappers::CulturalUnionMapper>("configurables/nationals.txt");
	religionMapper.scrapeCustomReligions();

	LOG(LogLevel::Info) << "*** Hello Vicky 2, creating world. ***";
	LOG(LogLevel::Info) << "-> Importing Provinces";
	importProvinces();
	Log(LogLevel::Progress) << "46 %";

	LOG(LogLevel::Info) << "-> Importing Vanilla Pops";
	importDefaultPops();
	Log(LogLevel::Progress) << "47 %";

	LOG(LogLevel::Info) << "-> Importing Potential Countries";
	importPotentialCountries();
	Log(LogLevel::Progress) << "48 %";

	LOG(LogLevel::Info) << "-> Loading Country Mapping Rules";
	countryMapper.createMappings(sourceWorld.getCultureGroupsMapper(), sourceWorld.getCountries(), provinceMapper);
	Log(LogLevel::Progress) << "49 %";

	LOG(LogLevel::Info) << "-> Loading Culture Mapping Rules";
	initializeCultureMappers();
	mappingChecker.check(sourceWorld, provinceMapper, religionMapper, cultureMapper);
	Log(LogLevel::Progress) << "50 %";

	LOG(LogLevel::Info) << "-> Pouring From Hollow Into Empty";
	cultureGroupsMapper.importNeoCultures(sourceWorld.getRegions(), sourceWorld.getCultureGroupsMapper(), cultureMapper);
	Log(LogLevel::Progress) << "51 %";

	LOG(LogLevel::Info) << "-> Converting Countries";
	convertCountries(sourceWorld, ideaEffectMapper);
	Log(LogLevel::Progress) << "52 %";

	LOG(LogLevel::Info) << "-> Converting Provinces";
	convertProvinces(sourceWorld, techGroupsMapper, sourceWorld.getRegions());
	Log(LogLevel::Progress) << "53 %";

	LOG(LogLevel::Info) << "-> Cataloguing Invasive Fauna";
	transcribeNeoCultures();
	Log(LogLevel::Progress) << "54 %";

	LOG(LogLevel::Info) << "-> Converting Diplomacy";
	diplomacy.convertDiplomacy(sourceWorld.getDiplomaticAgreements(), countryMapper, countries);
	diplomacy.sphereHRE(sourceWorld.decentralizedHRE(), getHreEmperor(), countries);
	Log(LogLevel::Progress) << "55 %";

	LOG(LogLevel::Info) << "-> Setting Up States";
	setupStates();
	Log(LogLevel::Progress) << "57 %";

	LOG(LogLevel::Info) << "-> Generating Unciv Reforms";
	convertUncivReforms(sourceWorld, techGroupsMapper);
	Log(LogLevel::Progress) << "58 %";

	LOG(LogLevel::Info) << "-> Converting Technology Levels";
	convertTechs(sourceWorld);
	Log(LogLevel::Progress) << "59 %";

	LOG(LogLevel::Info) << "-> Distributing Factories";
	allocateFactories(sourceWorld);
	Log(LogLevel::Progress) << "60 %";

	LOG(LogLevel::Info) << "-> Distributing Pops";
	setupPops(sourceWorld);
	Log(LogLevel::Progress) << "61 %";

	LOG(LogLevel::Info) << "-> Releasing Invasive Fauna Into Colonies";
	modifyPrimaryAndAcceptedCultures();
	Log(LogLevel::Progress) << "62 %";

	LOG(LogLevel::Info) << "-> Monitoring Native Fauna Reaction";
	addAcceptedCultures(sourceWorld.getRegions());
	Log(LogLevel::Progress) << "63 %";

	Log(LogLevel::Info) << "-> Dropping Infected AI Cores";
	dropCores();
	Log(LogLevel::Progress) << "64 %";

	Log(LogLevel::Info) << "-> Dropping Poorly-Shaped States";
	dropStates(techGroupsMapper);
	Log(LogLevel::Progress) << "65 %";

	LOG(LogLevel::Info) << "-> Merging Nations";
	decentralizeHRE(sourceWorld.decentralizedHRE(), getHreEmperor());
	addUnions(sourceWorld.decentralizedHRE(), getHreEmperor());

	Log(LogLevel::Info) << "-> Invoking the Undead";
	updateDeadNations();

	Log(LogLevel::Progress) << "66 %";

	LOG(LogLevel::Info) << "-> Converting Armies and Navies";
	convertArmies();
	Log(LogLevel::Progress) << "67 %";

	LOG(LogLevel::Info) << "-> Converting Ongoing Conflicts";
	convertWars(sourceWorld);
	Log(LogLevel::Progress) << "68 %";

	LOG(LogLevel::Info) << "-> Describing Religion";
	addReligionCulture();
	Log(LogLevel::Progress) << "69 %";

	LOG(LogLevel::Info) << "-> Converting Botanical Definitions";
	transcribeHistoricalData();
	Log(LogLevel::Progress) << "70 %";

	LOG(LogLevel::Info) << "-> Converting country flags";
	convertCountryFlags();
	Log(LogLevel::Progress) << "71 %";

	if (theConfiguration.isHpmEnabled())
	{
		LOG(LogLevel::Info) << "-> Update country details";
		updateCountryDetails();
		Log(LogLevel::Progress) << "72 %";
	}

	LOG(LogLevel::Info) << "---> Le Dump <---";
	output(versionParser);

	LOG(LogLevel::Info) << "*** Goodbye, Vicky 2, and godspeed. ***";
}

void V2::World::updateDeadNations()
{
	// We're updating dead nations with definitions from our predefined data file.
	for (const auto& [tag, country]: countries)
		if (country->getProvinces().empty()) // Dead as a doornail.
		{
			const auto& definition = deadDefinitionMapper.getDeadDefinitionForTag(tag);
			if (!definition)
				continue;
			if (definition->culture)
				country->setPrimaryCulture(*definition->culture);
			if (definition->religion)
				country->setReligion(*definition->religion);
			if (definition->government)
				country->setGovernment(*definition->government);
			if (definition->capital)
				country->setCapital(*definition->capital);
			if (definition->civilized)
				country->setCivilized(*definition->civilized);
			for (const auto& acceptedCulture: definition->acceptedCultures)
				country->addAcceptedCulture(acceptedCulture);
		}
}


void V2::World::addReligionCulture()
{
	for (const auto& country: countries)
	{
		std::string cultureReligion = "zzz_religion_";
		if (country.second->getReligion().empty())
			cultureReligion += "noreligion";
		else
			cultureReligion += country.second->getReligion();
		country.second->addAcceptedCulture(cultureReligion);
		country.second->addCountryFlag("religi");
	}
}

void V2::World::convertCountryFlags()
{
	const auto& theFlags = countryFlags.getFlags();
	for (const auto& country: countries)
	{
		if (const auto& srcCountry = country.second->getSourceCountry(); srcCountry)
		{
			for (const auto& flag: srcCountry->getFlags())
			{
				if (auto flagRangeItr = theFlags.equal_range(flag); flagRangeItr.first != theFlags.end())
				{
					while (flagRangeItr.first != flagRangeItr.second)
					{
						country.second->addCountryFlag(flagRangeItr.first->second);
						++flagRangeItr.first;
					}
				}
			}
		}
	}
}


void V2::World::dropStates(const mappers::TechGroupsMapper& techGroupsMapper)
{
	// States generally exist in undefined/untrusted (EU4) state unless they are colonies in progress, which are level 1.
	//
	// This function DROPS states to territory status if they do not contain civilized people.
	// If the owner is uncivilized, it RISES any states to states and will fastforward any colonies in progress to full states too.

	for (const auto& country: countries)
	{
		// Are we uncivilized?
		if (!country.second->isCivilized())
		{
			// State everything.
			for (const auto& state: country.second->getStates())
				state->setProvincesAsStates();
		}
		else
		{
			// We go state by state.
			for (const auto& state: country.second->getStates())
			{
				// First we look for cores and state the state if it contains containing our core(s).
				auto hasCore = false;
				for (const auto& province: state->getProvinces())
				{
					if (province->hasCore(country.first))
						hasCore = true;
				}
				if (hasCore)
				{
					state->setProvincesAsStates();
					continue;
				}

				// Otherwise, we need to look at the population. Do we have a westernization score for the dominant culture?
				auto hasCivilizedPeople = false;
				for (const auto& province: state->getProvinces())
				{
					const auto& majorityCulture = province->getDominantCulture();
					const auto score = techGroupsMapper.getWesternizationFromCulture(majorityCulture);
					if (score == 10)
						hasCivilizedPeople = true;
				}
				if (hasCivilizedPeople)
				{
					// A single province in a state containing civilized people is sufficient.
					state->setProvincesAsStates();
				}
				else
				{
					// We didn't find cores nor civilized people (or lack westernization scores), so we're dropping state to territory.
					state->setProvincesAsTerritories();
				}
			}
		}
	}
}

void V2::World::dropCores()
{
	// This function is used to drop EXTANT country cores over provinces where they do not have primary/accepted culture dominance.
	// Dead country cores will remain so something can be released.

	// This is quicker if we first build a country/culture cache and then check against it then iterate through
	// every province and do multiple unneeded checks.

	std::map<std::string, std::set<std::string>> theCache; // tag, accepted cultures cache
	std::set<std::string> deadCache;								 // dead countries where we need to preserve cores as they have no cultures.

	for (const auto& country: countries)
	{
		theCache[country.first] = country.second->getAcceptedCultures();
		if (!country.second->getPrimaryCulture().empty())
			theCache[country.first].insert(country.second->getPrimaryCulture());
		if (country.second->getProvinces().empty())
			deadCache.insert(country.first);
	}

	for (auto& province: provinces)
	{
		if (province.second->getCores().empty()) // Don't waste time
			continue;
		const auto dominantCulture = province.second->getDominantCulture();
		if (dominantCulture.empty()) // Let's not drop anything if we don't know what should remain.
			continue;
		std::set<std::string> survivingCores;

		for (const auto& core: province.second->getCores())
		{
			// Dead countries take priority.
			if (deadCache.count(core))
			{
				survivingCores.insert(core); // inserting automatically.
				continue;
			}

			const auto& cacheItr = theCache.find(core);
			if (cacheItr == theCache.end())
				continue;											// Dropping unrecognized core;
			if (cacheItr->second.count(dominantCulture)) // This province has core's (tag's) accepted culture, we can retain this core.
				survivingCores.insert(cacheItr->first);
		}
		province.second->replaceCores(survivingCores);
	}
}


void V2::World::addAcceptedCultures(const EU4::Regions& eu4Regions)
{
	// Accepted cultures at this stage only contain neocultures (if > 0.15) and ex-primary culture if neoculture took over.
	// We will do a census among our full cores and add only those accepted cultures that are relevant to us, eu4 and cultural
	// unions be damned. Vic2 cultural unions are exceptions and WILL add accepted cultures.

	for (const auto& country: countries)
	{
		if (country.second->getProvinces().empty())
			continue;								// don't disturb the dead
		std::map<std::string, long> census; // culture, population.

		// First, the census.
		for (const auto& province: country.second->getProvinces())
		{
			if (!province.second->hasCore(country.first))
				continue; // We don't census EU4/Vic2 territories.

			const auto& pops = province.second->getPopsForOutput();
			if (!pops)
				continue;
			for (const auto& pop: pops->second)
			{
				const auto culture = pop->getCulture();
				const auto size = pop->getSize();
				census[culture] += size;
			}
		}

		long totalPopulation = 0;
		for (const auto& entry: census)
			totalPopulation += entry.second;
		if (!totalPopulation)
			continue;

		const auto& primaryCulture = country.second->getPrimaryCulture();
		auto acceptedCultures = country.second->getAcceptedCultures();
		const auto& cultureGroup = cultureGroupsMapper.getGroupForCulture(primaryCulture);
		if (!cultureGroup)
			return;
		auto cultureGroupCultures = cultureGroup->getCultures();
		const auto sameGroupThreshold = acceptedCultureThresholdsMapper.getSameGroup();
		const auto foreignThreshold = acceptedCultureThresholdsMapper.getForeignGroup();

		for (const auto& culture: census)
		{
			if (culture.first == primaryCulture)
				continue;

			const auto& cultureItr = cultureGroupCultures.find(culture.first);
			const auto ratio = static_cast<double>(culture.second) / totalPopulation;
			if (cultureItr != cultureGroupCultures.end())
			{
				// This is a sameGroupCulture
				if (ratio >= sameGroupThreshold)
					acceptedCultures.insert(culture.first);
			}
			else
			{
				if (ratio >= foreignThreshold)
					acceptedCultures.insert(culture.first);
			}
		}

		// Now we look at vic2 cultural union exceptions.
		if (unionTagsMapper.isTagUnionfull(country.first))
		{
			// We'll add all cultures from unions.txt that consider this tag its cultural union.
			for (const auto& culture: culturalUnionMapper->getCulturesForCore(country.first))
			{
				if (culture != primaryCulture)
					acceptedCultures.insert(culture);
			}
		}

		// finally, for colonial nations, we need to ensure their overlord's mutated culture is accepted, to ease
		// cases where minority expulsion created monstrosities.

		if (!country.second->getColonyOverlord().empty())
		{
			const auto& overlordTag = country.second->getColonyOverlord();
			const auto& overlordItr = countries.find(overlordTag);
			if (overlordItr == countries.end())
			{
				Log(LogLevel::Warning) << country.first << " has an invalid overlord: " << overlordTag; // huh
				continue;
			}
			const auto& overlordPrimaryCulture = overlordItr->second->getSourceCountry()->getPrimaryCulture();
			const auto& overlordReligion = overlordItr->second->getSourceCountry()->getReligion();
			const auto& capital = country.second->getSourceCountry()->getCapital();
			const auto& eu4Tag = country.second->getSourceCountry()->getTag();
			const auto& overlordMutatedCulture = cultureMapper.cultureMatch(eu4Regions, overlordPrimaryCulture, overlordReligion, capital, eu4Tag);
			if (overlordMutatedCulture && primaryCulture != *overlordMutatedCulture)
				acceptedCultures.insert(*overlordMutatedCulture);
		}

		country.second->setAcceptedCultures(acceptedCultures);
	}
}

void V2::World::modifyPrimaryAndAcceptedCultures()
{
	// Key to understanding - this entire function is relevant only for extreme edge cases
	// where africans or other minority pops colonized new world and had neocultures generated
	// in large numbers. All the other cultures that have proper mappings for new world
	// will skip the majority of this process.

	// We're strolling through country provinces and registering how much of the
	// population has been swapped with neocultures. This is a fuzzy process because
	// multiple cultures mutate into single neoculture and only in some places, in others
	// they can transform into different base vic2 cultures.
	// We're updating primary/accepted only for significant global neoculture population (>15%).
	for (const auto& country: countries)
	{
		if (country.second->getProvinces().empty())
			continue; // don't disturb the dead

		std::map<std::string, long> census;								// culture, population.
		std::map<std::string, std::string> generatedNeoCultures; // orig culture, neoculture mapping

		// for countries without neocultures, stop wasting time.
		// Yankee, americano or vinlander are NOT neocultures.
		for (const auto& province: country.second->getProvinces())
		{
			if (!province.second->getGeneratedNeoCultures().empty())
			{
				auto tempCultures = province.second->getGeneratedNeoCultures();
				generatedNeoCultures.insert(tempCultures.begin(), tempCultures.end());
			}
		}
		if (generatedNeoCultures.empty())
			continue;

		// do a census
		for (const auto& province: country.second->getProvinces())
		{
			const auto& pops = province.second->getPopsForOutput();
			if (!pops)
				continue;
			for (const auto& pop: pops->second)
			{
				const auto culture = pop->getCulture();
				const auto size = pop->getSize();
				census[culture] += size;
			}
		}

		long totalPopulation = 0;
		for (const auto& entry: census)
			totalPopulation += entry.second;

		// Was any of the original eu4 cultures that mutated into neocultures a source for our current prim/acc cultures?
		auto primEU4Culture = country.second->getEU4PrimaryCulture();
		auto primV2Culture = country.second->getPrimaryCulture();
		auto accV2Cultures = country.second->getAcceptedCultures();

		const auto& primIter = generatedNeoCultures.find(primEU4Culture);
		if (primIter != generatedNeoCultures.end())
		{
			// is this our primary population now? Do we have modified pops overwhelming unmodified ones?
			if (census[generatedNeoCultures[primEU4Culture]] > census[primV2Culture])
			{
				// accept the reality of change. This can also happen if a small motherland integrates a large colony.
				accV2Cultures.insert(primV2Culture);
				primV2Culture = generatedNeoCultures[primEU4Culture];
			}
			else if (census[generatedNeoCultures[primEU4Culture]] > acceptedCultureThresholdsMapper.getSameGroup() * totalPopulation)
			{
				// Well, at least it's an accepted culture now. This is very possible if the motherland
				// assimilated a small colony on conversion.
				accV2Cultures.insert(generatedNeoCultures[primEU4Culture]);
			}
		}
		// Onto accepted cultures. This is simpler because we only add, no need to remove source cultures;
		// This is essentially like adding texans, americano or italoamericano to accepted cultures in usa.
		for (const auto& entry: census)
		{
			for (const auto& genCulture: generatedNeoCultures)
			{
				if (entry.first == genCulture.second) // this is a neoculture.
				{
					if (entry.second > acceptedCultureThresholdsMapper.getSameGroup() * totalPopulation && entry.first != primV2Culture)
					{
						accV2Cultures.insert(entry.first);
					}
				}
			}
		}
		// store and done.
		country.second->setPrimaryCulture(primV2Culture);
		country.second->setAcceptedCultures(accV2Cultures);
	}
}

void V2::World::transcribeNeoCultures()
{
	std::map<std::string, std::string> seenCultures;
	for (const auto& province: provinces)
	{
		auto seenNeoCultures = province.second->getGeneratedNeoCultures();
		for (const auto& seenNeoCulture: seenNeoCultures)
		{
			seenCultures.insert(std::make_pair(seenNeoCulture.second, province.second->getSuperRegion()));
		}
	}
	Log(LogLevel::Info) << "\tLocated " << seenCultures.size() << " new species.";
	for (const auto& culture: seenCultures)
	{
		auto workString = culture.first;
		// drop _culture
		auto position = workString.find("_culture");
		workString = workString.substr(0, position);

		position = workString.find("_" + culture.second);
		auto regionBit = culture.second;
		auto cultureBit = workString.substr(0, position);

		auto localizationLine = culture.first + ";";
		auto incLoc = regionLocalizations.getEnglishFor(cultureBit);
		if (incLoc)
			localizationLine += *incLoc + " ";
		incLoc = regionLocalizations.getEnglishFor(regionBit + "_adj");
		if (incLoc)
			localizationLine += *incLoc;
		localizationLine += ";";

		incLoc = regionLocalizations.getFrenchFor(cultureBit);
		if (incLoc)
			localizationLine += *incLoc + " ";
		incLoc = regionLocalizations.getFrenchFor(regionBit + "_adj");
		if (incLoc)
			localizationLine += *incLoc;
		localizationLine += ";";

		incLoc = regionLocalizations.getGermanFor(cultureBit);
		if (incLoc)
			localizationLine += *incLoc + " ";
		incLoc = regionLocalizations.getGermanFor(regionBit + "_adj");
		if (incLoc)
			localizationLine += *incLoc;
		localizationLine += ";;";

		incLoc = regionLocalizations.getSpanishFor(cultureBit);
		if (incLoc)
			localizationLine += *incLoc + " ";
		incLoc = regionLocalizations.getSpanishFor(regionBit + "_adj");
		if (incLoc)
			localizationLine += *incLoc;
		localizationLine += ";;;;;;;;;X";

		neoCultureLocalizations.insert(localizationLine);
	}
}


void V2::World::importProvinces()
{
	const auto provinceFileNames = discoverProvinceFilenames();
	for (const auto& provinceFileName: provinceFileNames)
	{
		if (provinceFileName.find(".txt") == std::string::npos)
			continue;
		auto newProvince = std::make_shared<Province>(provinceFileName, climateMapper, terrainDataMapper, provinceNameParser, navalBaseMapper);
		provinces.insert(std::make_pair(newProvince->getID(), newProvince));
	}

	if (theConfiguration.getRandomiseRgos())
	{
		shuffleRgos();
	}
}

std::set<std::string> V2::World::discoverProvinceFilenames()
{
	std::set<std::string> provinceFilenames;
	if (commonItems::DoesFolderExist("blankMod/output/history/provinces"))
	{
		provinceFilenames = commonItems::GetAllFilesInFolderRecursive("blankMod/output/history/provinces");
	}
	if (provinceFilenames.empty())
	{
		provinceFilenames = commonItems::GetAllFilesInFolderRecursive(theConfiguration.getVic2Path() + "/history/provinces");
	}

	return provinceFilenames;
}

void V2::World::shuffleRgos()
{
	if (bucketShuffler.empty())
	{
		LOG(LogLevel::Warning) << "No valid buckets defined, skipping RGO randomisation.";
		return;
	}

	LOG(LogLevel::Info) << "Shuffling RGOs in provinces.";
	for (auto& prov: provinces)
	{
		bucketShuffler.putInBucket(prov.second);
	}
	bucketShuffler.shuffle();
}


void V2::World::importDefaultPops()
{
	totalWorldPopulation = 0;
	std::string popsFolder;

	if (theConfiguration.isHpmEnabled())
	{
		popsFolder = theConfiguration.getVic2Path() + "/history/pops/1836.1.1/";
	}
	else
	{
		popsFolder = "./blankMod/output/history/pops/1836.1.1/";
	}
	auto filenames = commonItems::GetAllFilesInFolder(popsFolder);

	LOG(LogLevel::Info) << "Parsing minority pops mappings";

	for (const auto& filename: filenames)
	{
		importPopsFromFile(filename);
	}
}

void V2::World::importPopsFromFile(const std::string& filename)
{
	std::list<int> popProvinces;
	std::string popsFolder;

	if (theConfiguration.isHpmEnabled())
	{
		popsFolder = theConfiguration.getVic2Path() + "/history/pops/1836.1.1/";
	}
	else
	{
		// We are using our own defaults instead of vanilla source because it was modded with cultural minorities.
		popsFolder = "./blankMod/output/history/pops/1836.1.1/";
	}
	const mappers::PopMapper popMapper(popsFolder + filename);

	for (const auto& [provinceID, popDetails]: popMapper.getProvincePops())
	{
		popProvinces.push_back(provinceID);

		importPopsFromProvince(provinceID, popDetails);
	}
	popRegions.insert(std::make_pair(filename, popProvinces));
}

void V2::World::importPopsFromProvince(const int provinceID, const std::vector<mappers::PopDetails>& popsDetails)
{
	auto provincePopulation = 0;
	auto provinceSlavePopulation = 0;

	auto province = provinces.find(provinceID);
	if (province == provinces.end())
	{
		LOG(LogLevel::Warning) << "Could not find province " << provinceID << " for original pops.";
		return;
	}

	for (const auto& popDetails: popsDetails)
	{
		auto newPop = std::make_shared<Pop>(popDetails);
		if (minorityPopMapper.blankMajorityFromMinority(*newPop))
		{
			// If the pop we loaded had minority elements, their majority elements are now blank.
			// Ie. We've removed yankee from mormons, and we'll fill them later dynamically to become <majority_culture> mormons.
			province->second->addMinorityPop(newPop);
		}
		province->second->addVanillaPop(newPop);

		totalWorldPopulation += newPop->getSize();
		provincePopulation += newPop->getSize();
		if (newPop->isSlavePop())
			provinceSlavePopulation += newPop->getSize();
	}

	province->second->setSlaveProportion(1.0 * provinceSlavePopulation / provincePopulation);
}

void V2::World::importPotentialCountries()
{
	potentialCountries.clear();
	dynamicCountries.clear();

	std::ifstream v2CountriesInput;
	std::vector<std::string> countriesFiles;
	if (theConfiguration.isHpmEnabled())
	{
		countriesFiles.push_back(theConfiguration.getVic2Path() + "/common/countries.txt");
	}
	countriesFiles.push_back("./blankMod/output/common/countries.txt");

	for (const auto& countriesFile: countriesFiles)
	{
		v2CountriesInput.open(countriesFile);
		if (!v2CountriesInput.is_open())
			throw std::runtime_error("Could not open " + countriesFile + ". The converter may be corrupted, try downloading it again.");

		auto dynamicSection = false;
		while (!v2CountriesInput.eof())
		{
			std::string line;
			getline(v2CountriesInput, line);

			if (line[0] == '#' || line.size() < 3)
				continue;
			if (line.substr(0, 12) == "dynamic_tags")
			{
				dynamicSection = true;
				continue;
			}
			importPotentialCountry(line, dynamicSection);
		}
		v2CountriesInput.close();
	}
}

void V2::World::importPotentialCountry(const std::string& line, bool dynamicCountry)
{
	auto tag = line.substr(0, 3);


	if (!potentialCountries.contains(tag))
	{
		if (theConfiguration.isHpmEnabled() && (tag == "PAN" || tag == "PHI"))
			return;

		auto newCountry = std::make_shared<Country>(line, dynamicCountry, partyNameMapper, partyTypeMapper);
		potentialCountries.insert(std::make_pair(tag, newCountry));
		if (dynamicCountry && !dynamicCountries.contains(tag))
		{
			dynamicCountries.insert(std::make_pair(tag, newCountry));
		}
	}
}

void V2::World::initializeCultureMappers()
{
	LOG(LogLevel::Info) << "Parsing culture mappings.";
	cultureMapper.loadFile("configurables/culture_map.txt");

	LOG(LogLevel::Info) << "Parsing slave culture mappings.";
	slaveCultureMapper.loadFile("configurables/culture_map_slaves.txt");

	cultureGroupsMapper.initForV2();
}

void V2::World::convertCountries(const EU4::World& sourceWorld, const mappers::IdeaEffectMapper& ideaEffectMapper)
{
	initializeCountries(sourceWorld, ideaEffectMapper);
	LOG(LogLevel::Info) << "-> Converting National Values";
	convertNationalValues();
	LOG(LogLevel::Info) << "-> Converting Prestige";
	convertPrestige();
	LOG(LogLevel::Info) << "-> Adding Potential Countries";
	addAllPotentialCountries();
}

void V2::World::initializeCountries(const EU4::World& sourceWorld, const mappers::IdeaEffectMapper& ideaEffectMapper)
{
	for (const auto& sourceCountry: sourceWorld.getCountries())
	{
		const auto& V2Tag = countryMapper.getV2Tag(sourceCountry.first);
		if (!V2Tag)
			throw std::runtime_error("EU4 tag " + sourceCountry.first + " is unmapped and cannot be converted.");

		auto destCountry = createOrLocateCountry(*V2Tag, *sourceCountry.second);

		destCountry->initFromEU4Country(sourceWorld.getRegions(),
			 sourceCountry.second,
			 techSchoolMapper,
			 cultureMapper,
			 ideaEffectMapper,
			 religionMapper,
			 provinceMapper,
			 governmentMapper,
			 countryMapper);

		countries.insert(std::make_pair(*V2Tag, destCountry));
	}
}

std::shared_ptr<V2::Country> V2::World::createOrLocateCountry(const std::string& V2Tag, const EU4::Country& sourceCountry)
{
	std::shared_ptr<Country> destCountry;

	const auto& potentialCountry = potentialCountries.find(V2Tag);
	if (potentialCountry == potentialCountries.end())
	{
		auto countryFileName = sourceCountry.getName() + ".txt";
		destCountry = std::make_shared<Country>(V2Tag, countryFileName, partyNameMapper, partyTypeMapper);
	}
	else
	{
		destCountry = potentialCountry->second;
	}
	return destCountry;
}

bool scoresSorter(const std::pair<std::shared_ptr<V2::Country>, double>& first, const std::pair<std::shared_ptr<V2::Country>, double>& second)
{
	return first.second > second.second;
}

void V2::World::convertNationalValues()
{
	// set national values
	std::list<std::pair<std::shared_ptr<Country>, double>> libertyCountries;
	std::list<std::pair<std::shared_ptr<Country>, double>> equalityCountries;
	std::set<std::shared_ptr<Country>> countriesToSet;
	for (const auto& country: countries)
	{
		auto nvScores = country.second->getNationalValueScores();

		if (nvScores.liberty > nvScores.order)
		{
			libertyCountries.emplace_back(std::make_pair(country.second, nvScores.liberty));
		}
		if (nvScores.equality > nvScores.order && nvScores.equality > nvScores.liberty)
		{
			equalityCountries.emplace_back(std::make_pair(country.second, nvScores.equality));
		}
		countriesToSet.insert(country.second);
	}
	equalityCountries.sort(scoresSorter);

	// hardcoded so 5 countries will get to be equality oriented. Not really worth the trouble to put in a config file.
	auto equalityLeft = MAX_EQUALITY_COUNTRIES;
	for (const auto& equalCountry: equalityCountries)
	{
		if (equalityLeft < 1)
			break;
		const auto& countryToSetItr = countriesToSet.find(equalCountry.first);
		if (countryToSetItr != countriesToSet.end())
		{
			countriesToSet.erase(countryToSetItr);
			equalCountry.first->setNationalValue("nv_equality");
			equalityLeft--;
		}
	}

	// also hardcoded to 20 countries.
	libertyCountries.sort(scoresSorter);
	auto libertyLeft = MAX_LIBERTY_COUNTRIES;
	for (const auto& libertyCountry: libertyCountries)
	{
		if (libertyLeft < 1)
			break;
		const auto& countryToSetItr = countriesToSet.find(libertyCountry.first);
		if (countryToSetItr != countriesToSet.end())
		{
			countriesToSet.erase(countryToSetItr);
			libertyCountry.first->setNationalValue("nv_liberty");
			libertyLeft--;
		}
	}

	// The rest will default anyway.

	// Mod NVs override base game
	if (theConfiguration.isHpmEnabled())
	{
		for (const auto& [unused, country]: countries)
		{
			country->setHpmNationalValues();
		}
	}
}

void V2::World::convertPrestige()
{
	LOG(LogLevel::Info) << "-> Setting prestige";

	double highestScore = 0;
	for (const auto& country: countries)
	{
		double score = 0;
		auto srcCountry = country.second->getSourceCountry();
		if (srcCountry)
			score = srcCountry->getScore();
		if (score > highestScore)
			highestScore = score;
	}

	for (const auto& country: countries)
	{
		double score = 0;
		double prestige = 0;
		auto srcCountry = country.second->getSourceCountry();
		if (srcCountry)
			score = srcCountry->getScore();
		if (highestScore > 0)
			prestige = score / highestScore * 100.0;
		country.second->addPrestige(prestige);
	}
}

void V2::World::addAllPotentialCountries()
{
	// ALL potential countries should be output to the file, otherwise some things don't get initialized right when loading Vic2
	for (const auto& potentialCountry: potentialCountries)
	{
		const auto& countryItr = countries.find(potentialCountry.first);
		if (countryItr == countries.end())
		{
			// Note: This is a dead country.
			potentialCountry.second->initFromHistory(unreleasablesMapper, partyNameMapper, partyTypeMapper);
			countries.insert(make_pair(potentialCountry.first, potentialCountry.second));
		}
	}
}

unsigned int V2::World::countCivilizedNations() const
{
	unsigned int numPotentialGPs = 0;
	for (const auto& country: countries)
	{
		auto states = country.second->getStates();
		if (country.second->isCivilized() && states.size() > 1)
		{
			numPotentialGPs++;
		}
	}

	return numPotentialGPs;
}

void V2::World::convertProvinces(const EU4::World& sourceWorld, const mappers::TechGroupsMapper& techGroupsMapper, const EU4::Regions& eu4Regions)
{
	for (const auto& province: provinces)
	{
		auto eu4ProvinceNumbers = provinceMapper.getEU4ProvinceNumbers(province.first);
		if (eu4ProvinceNumbers.empty())
		{
			LOG(LogLevel::Warning) << "No mappings found for V2 province " << province.first << " (" << province.second->getName() << ")";
			// We leave it to defaults
			province.second->sterilizeProvince();
			continue;
		}
		if (std::count(eu4ProvinceNumbers.begin(), eu4ProvinceNumbers.end(), 0) > 0)
		{
			LOG(LogLevel::Warning) << "Invalid mapping found for V2 province " << province.first << " (" << province.second->getName() << ")";
			// We have a buggy mapping somewhere. Better drop.
			province.second->sterilizeProvince();
			continue;
		}

		auto eu4Owner = determineProvinceOwnership(eu4ProvinceNumbers, sourceWorld);
		if (!eu4Owner)
		{
			// Probably un-colonized province; blank it and leave it to defaults.
			province.second->sterilizeProvince();
			continue;
		}
		// For controller we're FAR less picky. Scroll through the provinces and see if anyone has majority.
		auto eu4Controller = determineProvinceControllership(eu4ProvinceNumbers, sourceWorld);

		// Remap owner to something V2 can understand
		auto possibleOwner = countryMapper.getV2Tag(*eu4Owner);
		if (!possibleOwner)
			throw std::runtime_error("Error mapping EU4 tag " + *eu4Owner + " to a Vic2 tag! (V2 Province " + std::to_string(province.first) + ")");
		auto owner = *possibleOwner;

		auto possibleController = countryMapper.getV2Tag(*eu4Controller);
		if (!possibleController)
			throw std::runtime_error("Error mapping EU4 tag " + *eu4Controller + " to a Vic2 tag! (V2 Province " + std::to_string(province.first) + ")");
		const auto& controller = *possibleController;

		province.second->setOwner(owner);
		province.second->setController(controller);

		const auto& ownerCountry = countries.find(owner);
		if (ownerCountry == countries.end())
		{
			Log(LogLevel::Warning) << "Province " << province.first << " owner " << owner << " has no country.";
			continue;
		}
		if (theConfiguration.getAfricaReset() == Configuration::AFRICARESET::ResetAfrica)
		{
			if (africaResetMapper.isTechResettable(techGroupsMapper.getWesternizationFromTechGroup(ownerCountry->second->getSourceCountry()->getTechGroup())))
			{
				auto resetProvince = false;
				// A single match will trip the province into sterilization.
				for (const auto& origProvinceID: eu4ProvinceNumbers)
				{
					const auto& areaName = eu4Regions.getParentAreaName(origProvinceID);
					const auto& regionName = eu4Regions.getParentRegionName(origProvinceID);
					const auto& superRegionName = eu4Regions.getParentSuperRegionName(origProvinceID);
					if (!areaName || !regionName || !superRegionName)
						continue;
					if (africaResetMapper.isRegionResettable(*areaName) || africaResetMapper.isRegionResettable(*regionName) ||
						 africaResetMapper.isRegionResettable(*superRegionName))
						resetProvince = true;
				}
				if (resetProvince)
				{
					province.second->sterilizeProvince();
					continue;
				}
			}
		}

		if (ownerCountry != countries.end())
		{
			ownerCountry->second->addProvince(province.second);
		}

		// Before we convert a province, we need to filter those eu4 province sources belonging to another owner.
		// ... don't want to influence development with filthy foreign manufactories and forts.
		std::vector<std::shared_ptr<EU4::Province>> filteredSources;
		for (const auto& eu4provID: eu4ProvinceNumbers)
		{
			if (sourceWorld.getProvince(eu4provID)->getOwnerString() == *eu4Owner)
			{
				filteredSources.push_back(sourceWorld.getProvince(eu4provID));
			}
		}

		province.second->convertFromOldProvince(filteredSources,
			 sourceWorld.getCountries(),
			 sourceWorld.getRegions(),
			 cultureMapper,
			 slaveCultureMapper,
			 continentsMapper,
			 religionMapper,
			 countryMapper,
			 provinceMapper,
			 sourceWorld.decentralizedHRE());
	}
}

std::optional<std::string> V2::World::determineProvinceOwnership(const std::set<int>& eu4ProvinceNumbers, const EU4::World& sourceWorld) const
{
	// determine ownership by province development.
	std::map<std::string, std::vector<std::shared_ptr<EU4::Province>>> theClaims; // tag, claimed provinces
	std::map<std::string, std::pair<int, int>> theShares;									// tag, development/tax

	for (auto eu4ProvinceID: eu4ProvinceNumbers)
	{
		const auto& eu4province = sourceWorld.getProvince(eu4ProvinceID);
		auto ownerTag = eu4province->getOwnerString();
		if (ownerTag.empty())
			continue; // Don't touch un-colonized provinces.
		theClaims[ownerTag].push_back(eu4province);
		theShares[ownerTag] = std::make_pair(lround(eu4province->getProvinceWeight()), lround(eu4province->getBaseTax()));
	}
	// Let's see who the lucky winner is.
	std::string winner;
	auto maxDev = 0;
	auto maxTax = 0;
	for (const auto& share: theShares)
	{
		if (share.second.first > maxDev)
		{
			winner = share.first;
			maxDev = share.second.first;
			maxTax = share.second.second;
		}
		if (share.second.first == maxDev && share.first != winner)
		{
			// We have a tie
			if (share.second.second > maxTax)
			{
				winner = share.first;
				maxDev = share.second.first;
				maxTax = share.second.second;
			}
			if (share.second.second == maxTax)
			{
				// Shit. Check for core?
				for (const auto& claim: theClaims[share.first])
					if (!claim->isTerritorialCore())
					{
						// It's a full core of someone. Might as well take it. Will not resolve further ties, thank you.
						winner = share.first;
						maxDev = share.second.first;
						maxTax = share.second.second;
					}
			}
		}
	}
	if (winner.empty())
		return std::nullopt;
	return std::move(winner);
}

std::optional<std::string> V2::World::determineProvinceControllership(const std::set<int>& eu4ProvinceNumbers, const EU4::World& sourceWorld)
{
	// determine ownership by pure numbers. Errors due to equal numbers can be assigned to war uncertainty and fog of war. *shrug*
	std::map<std::string, std::vector<int>> theClaims; // tag, claimed provinces

	for (auto eu4ProvinceID: eu4ProvinceNumbers)
	{
		const auto& eu4province = sourceWorld.getProvince(eu4ProvinceID);
		auto controllerTag = eu4province->getControllerString();
		if (controllerTag.empty())
			continue; // Don't touch un-colonized provinces.
		theClaims[controllerTag].push_back(eu4ProvinceID);
	}
	// Let's see who the lucky winner is.
	std::string winner;
	unsigned int maxCount = 0;
	for (const auto& tag: theClaims)
	{
		if (tag.second.size() > maxCount)
		{
			maxCount = static_cast<int>(tag.second.size());
			winner = tag.first;
		}
	}
	if (winner.empty())
		return std::nullopt;
	return std::move(winner);
}

void V2::World::setupStates()
{
	std::list<std::shared_ptr<Province>> unassignedProvs;
	for (const auto& province: provinces)
		unassignedProvs.push_back(province.second);

	while (!unassignedProvs.empty())
	{
		auto iter = unassignedProvs.begin();
		const auto provId = (*iter)->getID();
		auto owner = (*iter)->getOwner();

		if (owner.empty())
		{
			unassignedProvs.erase(iter);
			continue;
		}

		auto newState = std::make_shared<State>(stateId, *iter);
		stateId++;
		auto neighbors = stateMapper.getAllProvincesInState(provId);

		// We are breaking states apart according to colonial status. This is so primitives can retain
		// their full states next to colonizers who have colonial provinces in the same state.
		// This ALSO means multiple naval bases within apparently single state.
		const auto colonial = (*iter)->isColony();
		newState->setColonial(colonial);
		iter = unassignedProvs.erase(iter);

		for (const auto& neighborID: neighbors)
		{
			for (iter = unassignedProvs.begin(); iter != unassignedProvs.end(); ++iter)
			{
				if ((*iter)->getID() == neighborID)
				{
					if ((*iter)->getOwner() == owner)
					{
						if ((*iter)->isColony() == colonial)
						{
							newState->addProvince(*iter);
							iter = unassignedProvs.erase(iter);
						}
					}
				}
			}
		}

		newState->rebuildNavalBase();
		const auto& iter2 = countries.find(owner);
		if (iter2 != countries.end())
		{
			iter2->second->addState(newState, portProvincesMapper);
		}
	}
}

void V2::World::convertUncivReforms(const EU4::World& sourceWorld, const mappers::TechGroupsMapper& techGroupsMapper)
{
	// tech group
	auto techGroupAlgorithm = CIV_ALGORITHM::newer;
	double topTech = 0;
	auto topInstitutions = 0;
	const auto version18 = GameVersion("1.18.0");

	if (sourceWorld.getVersion() >= version18)
	{
		// Find global max tech and institutions embraced

		for (const auto& country: sourceWorld.getCountries())
		{
			if (country.second->getProvinces().empty())
				continue;
			const auto totalTechs = country.second->getMilTech() + country.second->getAdmTech() + country.second->getDipTech();
			if (totalTechs > topTech)
				topTech = totalTechs;
			const auto currInstitutions = country.second->numEmbracedInstitutions();
			if (currInstitutions > topInstitutions)
				topInstitutions = currInstitutions;
		}
	}
	else
	{
		// Hardcoded for best monolithic experience.
		topTech = 96;
		topInstitutions = 7;
		techGroupAlgorithm = CIV_ALGORITHM::older;
	}

	for (const auto& country: countries)
	{
		country.second->convertUncivReforms(techGroupAlgorithm, topTech, topInstitutions, techGroupsMapper);
	}

	// inherit civilization level for landless countries from their capital's owner
	for (const auto& country: countries)
	{
		if (!country.second->getProvinces().empty())
			continue;
		const auto capitalID = country.second->getCapital();
		if (capitalID == 0)
			continue;
		const auto& capital = getProvince(capitalID);
		std::string capOwnerTag = capital->getOwner();
		std::shared_ptr<Country> capOwner = getCountry(capOwnerTag);
		if (!capOwner)
			continue;
		country.second->convertLandlessReforms(*capOwner);
	}
}

void V2::World::convertTechs(const EU4::World& sourceWorld)
{
	const helpers::TechValues techValues(countries);

	for (const auto& country: countries)
	{
		if (helpers::TechValues::isValidCountryForTechConversion(*country.second))
		{
			const auto armyScore = techValues.getNormalizedArmyTech(*country.second->getSourceCountry());
			const auto navyScore = techValues.getNormalizedNavyTech(*country.second->getSourceCountry());
			const auto cultureScore = techValues.getNormalizedCultureTech(*country.second->getSourceCountry());
			const auto industryScore = techValues.getNormalizedIndustryTech(*country.second->getSourceCountry());
			const auto commerceScore = techValues.getNormalizedCommerceTech(*country.second->getSourceCountry());
			country.second->setTechs(startingTechMapper, startingInventionMapper, armyScore, navyScore, cultureScore, industryScore, commerceScore);
		}
	}
}


void V2::World::allocateFactories(const EU4::World& sourceWorld)
{
	// determine average production tech
	double admMean = 0;
	auto num = 1;
	for (const auto& country: sourceWorld.getCountries())
	{
		if (country.second->getProvinces().empty())
			continue;

		const auto admTech = country.second->getAdmTech();
		admMean += (admTech - admMean) / num;
		++num;
	}

	// give all extant civilized nations an industrial score
	std::deque<std::pair<double, std::shared_ptr<Country>>> weightedCountries;
	for (const auto& country: countries)
	{
		if (!country.second->isCivilized())
			continue;

		const auto& sourceCountry = country.second->getSourceCountry();
		if (!sourceCountry)
			continue;
		if (country.second->getProvinces().empty())
			continue;

		// modified manufactory weight follows diminishing returns curve y = x^(3/4)+log((x^2)/5+1)
		const auto manuCount = sourceCountry->getManufactoryCount();
		const auto manuWeight = pow(manuCount, 0.75) + log1p(static_cast<double>(pow(manuCount, 2)) / 5.0);
		const auto manuDensity = sourceCountry->getManufactoryDensity();
		auto industryWeight = (sourceCountry->getAdmTech() - admMean) * 5 + manuWeight * manuDensity;
		// having one manufactory and average tech is not enough; you must have more than one, or above-average tech
		if (industryWeight > 1.0)
		{
			weightedCountries.emplace_back(std::pair<double, std::shared_ptr<Country>>(industryWeight, country.second));
		}
	}

	if (weightedCountries.empty())
	{
		LOG(LogLevel::Warning) << "No countries are able to accept factories? What's the state of tech in this world?";
		return;
	}

	sort(weightedCountries.begin(),
		 weightedCountries.end(),
		 [](const std::pair<double, std::shared_ptr<Country>>& country1, const std::pair<double, std::shared_ptr<Country>>& country2) {
			 return country1.first > country2.first;
		 });

	// allow a maximum of 15 (plus any tied at fifteenth place) countries to receive factories
	std::deque<std::pair<double, std::shared_ptr<Country>>> restrictCountries;
	auto threshold = 1.0;
	auto totalIndWeight = 0.0;
	for (const auto& country: weightedCountries)
	{
		if (restrictCountries.size() > 15 && country.first < threshold - FLT_EPSILON)
			break;

		restrictCountries.push_back(country); // preserve sort
		totalIndWeight += country.first;
		threshold = country.first;
	}

	if (totalIndWeight == 0.0)
	{
		LOG(LogLevel::Warning) << "No factories for anyone! Industry levels are too unified - are you converting a starting date?";
		return;
	}

	weightedCountries.swap(restrictCountries);

	// remove nations that won't have enough industrial score for even one factory
	auto factoryList = factoryTypeMapper.buildFactories();
	while (lround(weightedCountries.begin()->first / totalIndWeight * static_cast<double>(factoryList.size())) < 1.0)
	{
		weightedCountries.pop_front();
		if (weightedCountries.empty())
		{
			LOG(LogLevel::Warning) << "These are all primitives! No factories for anyone!";
			return;
		}
	}

	// determine how many factories each eligible nation gets
	std::vector<std::pair<int, std::shared_ptr<Country>>> factoryCounts;
	for (const auto& country: weightedCountries)
	{
		int factories = lround(country.first / totalIndWeight * static_cast<double>(factoryList.size()));
		factoryCounts.emplace_back(std::pair<int, std::shared_ptr<Country>>(factories, country.second));
	}

	// allocate the factories
	auto lastReceptiveCountry = factoryCounts.end()--;
	auto citr = factoryCounts.begin();
	while (!factoryList.empty())
	{
		auto accepted = false;
		if (citr->first > 0) // can take more factories
		{
			for (auto qitr = factoryList.begin(); qitr != factoryList.end(); ++qitr)
			{
				if (citr->second->addFactory(*qitr))
				{
					--(citr->first);
					lastReceptiveCountry = citr;
					accepted = true;
					factoryList.erase(qitr);
					break;
				}
			}
		}
		if (!accepted && citr == lastReceptiveCountry)
		{
			break;
		}
		if (++citr == factoryCounts.end())
		{
			citr = factoryCounts.begin(); // loop around to beginning
		}
	}
}

void V2::World::setupPops(const EU4::World& sourceWorld)
{
	const auto my_totalWorldPopulation = totalWorldPopulation; // Extreme will redistribute vanilla pops.
	auto totalProvinceWeight = 0.0;
	for (const auto& [countryID, country]: countries)
		for (const auto& [provinceID, province]: country->getProvinces())
			totalProvinceWeight += province->getProvinceWeight();
	const auto popWeightRatio = my_totalWorldPopulation / totalProvinceWeight; // This is relevant only for extreme reshaping.

	CIV_ALGORITHM popAlgorithm;
	const auto version12 = GameVersion("1.12.0");
	if (sourceWorld.getVersion() >= version12)
	{
		popAlgorithm = CIV_ALGORITHM::newer;
	}
	else
	{
		popAlgorithm = CIV_ALGORITHM::older;
	}

	for (const auto& country: countries)
		country.second->setupPops(popWeightRatio, popAlgorithm, provinceMapper);

	LOG(LogLevel::Info) << "Vanilla world population: " << totalWorldPopulation;
	if (theConfiguration.getPopShaping() == Configuration::POPSHAPES::Extreme)
	{
		LOG(LogLevel::Info) << "\tTotal world weight sum: " << totalProvinceWeight << " (dev + buildings)";
		LOG(LogLevel::Info) << "\tPopulation per weight point is: " << my_totalWorldPopulation << " / " << totalProvinceWeight << " = " << popWeightRatio;
	}
	long newTotalPopulation = 0;
	for (const auto& province: provinces)
		newTotalPopulation += province.second->getTotalPopulation();
	LOG(LogLevel::Info) << "New total world population: " << newTotalPopulation;
}

void V2::World::addUnions(bool hreDecentralized, const std::shared_ptr<Country>& emperor)
{
	if (theConfiguration.getCoreHandling() == Configuration::COREHANDLES::DropAll)
		return;

	LOG(LogLevel::Info) << "Distributing national and cultural union cores.";
	std::shared_ptr<mappers::CultureGroup> emperorCultureGroup;
	if (emperor)
		emperorCultureGroup = cultureGroupsMapper.getGroupForCulture(emperor->getPrimaryCulture());

	for (const auto& province: provinces)
	{
		if (!province.second->wasColony())
		{
			auto cultures = province.second->getCulturesOverThreshold(0.5);
			for (const auto& culture: cultures)
			{
				auto unionCores = culturalUnionMapper->getCoresForCulture(culture);
				auto nationalCores = culturalNationalitiesMapper->getCoresForCulture(culture);
				if (hreDecentralized && emperorCultureGroup->containsCulture(culture))
					unionCores.clear();

				switch (theConfiguration.getCoreHandling())
				{
					case Configuration::COREHANDLES::DropNational:
						for (const auto& core: unionCores)
							province.second->addCore(core);
						break;
					case Configuration::COREHANDLES::DropUnions:
						for (const auto& core: nationalCores)
							province.second->addCore(core);
						break;
					case Configuration::COREHANDLES::DropNone:
						for (const auto& core: unionCores)
							province.second->addCore(core);
						for (const auto& core: nationalCores)
							province.second->addCore(core);
						break;
					case Configuration::COREHANDLES::DropAll:
						break;
				}
			}
		}
	}
}

void V2::World::decentralizeHRE(bool hreDecentralized, const std::shared_ptr<Country>& emperor)
{
	if (hreDecentralized && emperor)
		cultureGroupsMapper.getGroupForCulture(emperor->getPrimaryCulture())->clearUnionTag();
}

std::shared_ptr<V2::Country> V2::World::getHreEmperor() const
{
	for (const auto& [unused, country]: countries)
	{
		if (country->isEmperorHRE())
			return country;
	}
	return nullptr;
}


void V2::World::convertArmies()
{
	// convert leaders and armies
	for (const auto& country: countries)
	{
		country.second->convertLeaders(leaderTraitMapper);
		country.second->convertArmies(regimentCostsMapper, provinces, portProvincesMapper, provinceMapper);
	}
}

void V2::World::convertWars(const EU4::World& sourceWorld)
{
	for (const auto& eu4War: sourceWorld.getWars())
	{
		War newWar;
		if (newWar.loadWar(eu4War, warGoalMapper, provinceMapper, countryMapper, countries))
		{
			wars.push_back(newWar);
		}
		else
		{
			Log(LogLevel::Warning) << "Failed to transcribe war: " << eu4War.getName();
		}
	}
}

void V2::World::output(const mappers::VersionParser& versionParser) const
{
	commonItems::TryCreateFolder("output");
	Log(LogLevel::Progress) << "80 %";

	LOG(LogLevel::Info) << "<- Copying Mod Template";
	commonItems::CopyFolder("blankMod/output", "output/output");
	Log(LogLevel::Progress) << "81 %";

	LOG(LogLevel::Info) << "<- Moving Mod Template >> " << theConfiguration.getOutputName();
	commonItems::RenameFolder("output/output", "output/" + theConfiguration.getOutputName());
	Log(LogLevel::Progress) << "82 %";

	LOG(LogLevel::Info) << "<- Crafting .mod File";
	createModFile();
	Log(LogLevel::Progress) << "83 %";

	// Record converter version
	LOG(LogLevel::Info) << "<- Writing version";
	outputVersion(versionParser);
	Log(LogLevel::Progress) << "84 %";

	// Update bookmark starting dates
	LOG(LogLevel::Info) << "<- Updating bookmarks";
	modifyDefines();
	Log(LogLevel::Progress) << "85 %";

	// Create common\countries path.
	const auto countriesPath = "output/" + theConfiguration.getOutputName() + "/common/countries";
	if (!commonItems::TryCreateFolder(countriesPath))
	{
		return;
	}
	Log(LogLevel::Progress) << "86 %";

	// Output common\countries.txt
	LOG(LogLevel::Info) << "<- Creating countries.txt";
	outputCommonCountries();
	Log(LogLevel::Progress) << "87 %";

	// Create flags for all new countries.
	LOG(LogLevel::Info) << "-> Creating Flags";
	Flags flags;
	Log(LogLevel::Progress) << "88 %";

	LOG(LogLevel::Info) << "-> Setting Flags";
	flags.setV2Tags(countries, countryMapper);
	Log(LogLevel::Progress) << "89 %";

	LOG(LogLevel::Info) << "<- Writing Flags";
	flags.output();
	Log(LogLevel::Progress) << "90 %";

	// Create localizations for all new countries. We don't actually know the names yet so we just use the tags as the names.
	LOG(LogLevel::Info) << "<- Writing Localisation Text";
	outputLocalisation();
	Log(LogLevel::Progress) << "91 %";

	LOG(LogLevel::Info) << "<- Writing Provinces";
	outputProvinces();
	Log(LogLevel::Progress) << "92 %";

	LOG(LogLevel::Info) << "<- Writing Countries";
	outputCountries();
	Log(LogLevel::Progress) << "93 %";

	LOG(LogLevel::Info) << "<- Writing Diplomacy";
	diplomacy.output();
	Log(LogLevel::Progress) << "94 %";

	LOG(LogLevel::Info) << "<- Writing Armed and Unarmed Conflicts";
	outputWars();
	Log(LogLevel::Progress) << "95 %";

	LOG(LogLevel::Info) << "<- Writing Culture Definitions";
	outputCultures();
	Log(LogLevel::Progress) << "96 %";

	LOG(LogLevel::Info) << "<- Writing Religion Definitions";
	outputReligions();

	LOG(LogLevel::Info) << "<- Writing Pops";
	outputPops();
	Log(LogLevel::Progress) << "97 %";

	LOG(LogLevel::Info) << "<- Sending Botanical Expedition";
	outputHistory();
	Log(LogLevel::Progress) << "98 %";

	LOG(LogLevel::Info) << "<- Writing Treatise on the Origins of Invasive Fauna";
	outputNeoCultures();
	Log(LogLevel::Progress) << "99 %";

	if (theConfiguration.isHpmEnabled())
	{
		copyHpmFiles();
		outputGTFO(countries);
		outputReturnCores(countries);
		Log(LogLevel::Error) << "HPM Hybrid created. Do NOT enable HPM in Vic2 launcher, just this mod.";
	}

	// verify countries got written
	LOG(LogLevel::Info) << "-> Verifying All Countries Written";
	verifyCountriesWritten();
}

void V2::World::outputNeoCultures() const
{
	std::ofstream output("output/" + theConfiguration.getOutputName() + "/localisation/0_Neocultures.csv");
	if (!output.is_open())
		throw std::runtime_error("Could not create neocultures file!");

	output << "KEY;ENGLISH;FRENCH;GERMAN;POLISH;SPANISH;ITALIAN;HUNGARIAN;CZECH;HUNGARIAN;DUTCH;PORTUGUESE;RUSSIAN;FINNISH;X\n";
	for (const auto& line: neoCultureLocalizations)
	{
		output << line << "\n";
	}
	output.close();
}


void V2::World::transcribeHistoricalData()
{
	std::vector<std::pair<std::string, EU4::HistoricalEntry>> transcribedData;
	for (const auto& entry: historicalData)
	{
		const auto& possibleTag = countryMapper.getV2Tag(entry.first);
		if (possibleTag)
			transcribedData.emplace_back(std::make_pair(*possibleTag, entry.second));
	}
	historicalData.swap(transcribedData);
}

void V2::World::outputHistory() const
{
	std::ofstream output("output/" + theConfiguration.getOutputName() + "/common/botanical_expedition.txt");
	if (!output.is_open())
	{
		throw std::runtime_error("Could not send botanical expedition output/" + theConfiguration.getOutputName() + "/common/botanical_expedition.txt - " +
										 commonItems::GetLastErrorString());
	}
	output << historicalData;
	output.close();
}

void V2::World::outputCultures() const
{
	std::ofstream output("output/" + theConfiguration.getOutputName() + "/common/cultures.txt");
	if (!output.is_open())
	{
		throw std::runtime_error(
			 "Could not create cultures file at output/" + theConfiguration.getOutputName() + "/common/cultures.txt - " + commonItems::GetLastErrorString());
	}
	output << cultureGroupsMapper;
	output.close();
}


void V2::World::outputReligions() const
{
	std::ofstream output("output/" + theConfiguration.getOutputName() + "/common/religion.txt");
	if (!output.is_open())
	{
		throw std::runtime_error(
			 "Could not create religions file at output/" + theConfiguration.getOutputName() + "/common/religion.txt - " + commonItems::GetLastErrorString());
	}
	output << religiousGroupsMapper;
	output.close();
}


void V2::World::outputWars() const
{
	commonItems::TryCreateFolder("output/" + theConfiguration.getOutputName() + "/history/wars");
	for (const auto& war: wars)
	{
		const auto& filename = war.generateFileName();
		std::ofstream output("output/" + theConfiguration.getOutputName() + "/history/wars/" + filename);
		if (!output.is_open())
		{
			throw std::runtime_error(
				 "Could not create wars file output/" + theConfiguration.getOutputName() + "/history/wars/" + filename + " - " + commonItems::GetLastErrorString());
		}
		output << war;
		output.close();
	}
}

void V2::World::outputVersion(const mappers::VersionParser& versionParser)
{
	std::ofstream output("output/" + theConfiguration.getOutputName() + "/eu4tov2_version.txt");
	if (!output.is_open())
		throw std::runtime_error("Error writing version file! Is the output folder writable?");
	output << versionParser;
	output.close();
}

void V2::World::outputCommonCountries() const
{
	std::ofstream output("output/" + theConfiguration.getOutputName() + "/common/countries.txt");
	if (!output.is_open())
		throw std::runtime_error("Could not create countries file!");

	for (const auto& country: countries)
	{
		const auto& dynamic = dynamicCountries.find(country.first);
		// First output all regular countries, order matters!
		if (dynamic == dynamicCountries.end())
		{
			output << country.first << " = \"countries/" << clipCountryFileName(country.second->getCommonCountryFile()) << "\"\n";
		}
	}
	output << "\n";
	output << "##HoD Dominions\n";
	output << "dynamic_tags = yes # any tags after this is considered dynamic dominions\n";
	for (const auto& country: dynamicCountries)
	{
		output << country.first << " = \"countries/" << clipCountryFileName(country.second->getCommonCountryFile()) << "\"\n";
	}
	output.close();
}

void V2::World::outputLocalisation() const
{
	auto localisationPath = "output/" + theConfiguration.getOutputName() + "/localisation";
	if (!commonItems::TryCreateFolder(localisationPath))
		return;

	auto source = theConfiguration.getVic2Path() + "/localisation/text.csv";
	auto dest = localisationPath + "/text.csv";

	LOG(LogLevel::Info) << "<- Writing Localization Names";
	std::ofstream output(localisationPath + "/0_Names.csv", std::ofstream::app);
	if (!output.is_open())
		throw std::runtime_error("Could not update localization text file");

	commonItems::TryCreateFolder("output/" + theConfiguration.getOutputName() + "/history/countries");
	commonItems::TryCreateFolder("output/" + theConfiguration.getOutputName() + "/history/units");
	for (const auto& country: countries)
	{
		if (country.second->isNewCountry())
		{
			output << country.second->getLocalisation();
		}
	}
	output.close();
}

void V2::World::outputProvinces() const
{
	commonItems::TryCreateFolder("output/" + theConfiguration.getOutputName() + "/history/provinces");
	for (const auto& province: provinces)
	{
		auto filename = province.second->getFilename();
		auto path = getPath(filename);
		if (!commonItems::TryCreateFolder("output/" + theConfiguration.getOutputName() + "/history/provinces/" + path))
			throw std::runtime_error("Could not create directory: output/" + theConfiguration.getOutputName() + "/history/provinces/" + path);

		std::ofstream output("output/" + theConfiguration.getOutputName() + "/history/provinces/" + filename);
		if (!output.is_open())
		{
			throw std::runtime_error("Could not create province history file output/" + theConfiguration.getOutputName() + "/history/provinces/" + filename +
											 " - " + commonItems::GetLastErrorString());
		}
		output << *province.second;
		output.close();
	}
}

void V2::World::outputCountries() const
{
	for (const auto& country: countries)
	{
		// Country file
		if (!country.second->isDynamicCountry())
		{
			std::ofstream output("output/" + theConfiguration.getOutputName() + "/history/countries/" + clipCountryFileName(country.second->getFilename()));
			if (!output.is_open())
				throw std::runtime_error("Could not create country history file " + clipCountryFileName(country.second->getFilename()));
			output << *country.second;
			output.close();
		}
		// commons file
		std::ofstream commons("output/" + theConfiguration.getOutputName() + "/common/countries/" + clipCountryFileName(country.second->getCommonCountryFile()));
		if (!commons.is_open())
			throw std::runtime_error(
				 "Could not open output/" + theConfiguration.getOutputName() + "/common/countries/" + clipCountryFileName(country.second->getCommonCountryFile()));
		country.second->outputCommons(commons);
		commons.close();
		// OOB
		std::ofstream output("output/" + theConfiguration.getOutputName() + "/history/units/" + country.first + "_OOB.txt");
		if (!output.is_open())
			throw std::runtime_error("Could not create OOB file " + country.first + "_OOB.txt");
		country.second->outputOOB(output);
	}
}

void V2::World::modifyDefines() const
{
	auto potentialGPs = countCivilizedNations();
	std::string startDate = "<STARTDATE>";
	std::string numGPs = "GREAT_NATIONS_COUNT = 8";
	std::ostringstream incomingDefines, incomingBookmarks;

	// Edit starting date in defines + adjust GP count if needed
	if (theConfiguration.isHpmEnabled())
	{
		bool definesRemoved = fs::remove("output/" + theConfiguration.getOutputName() + "/common/defines.lua");
		if (definesRemoved)
			fs::copy_file("configurables/HPM/common/defines.lua", "output/" + theConfiguration.getOutputName() + "/common/defines.lua");
		else
			throw std::runtime_error("Could not replace defines.lua");
	}
	std::ifstream defines_lua("output/" + theConfiguration.getOutputName() + "/common/defines.lua");
	incomingDefines << defines_lua.rdbuf();
	defines_lua.close();
	auto strDefines = incomingDefines.str();
	auto pos1 = strDefines.find(startDate);
	strDefines.replace(pos1, startDate.length(), theConfiguration.getLastEU4Date().toString());

	if (potentialGPs < 8)
	{
		auto posGPs = strDefines.find(numGPs);
		auto replacementGPs = "GREAT_NATIONS_COUNT = " + std::to_string(potentialGPs);
		strDefines.replace(posGPs, numGPs.length(), replacementGPs);
	}

	std::ofstream out_defines_lua("output/" + theConfiguration.getOutputName() + "/common/defines.lua");
	out_defines_lua << strDefines;
	out_defines_lua.close();

	// Edit bookmark start
	std::ifstream bookmarks_txt("output/" + theConfiguration.getOutputName() + "/common/bookmarks.txt");
	incomingBookmarks << bookmarks_txt.rdbuf();
	bookmarks_txt.close();
	auto strBookmarks = incomingBookmarks.str();
	auto pos2 = strBookmarks.find(startDate);
	strBookmarks.replace(pos2, startDate.length(), theConfiguration.getLastEU4Date().toString());
	std::ofstream out_bookmarks_txt("output/" + theConfiguration.getOutputName() + "/common/bookmarks.txt");
	out_bookmarks_txt << strBookmarks;
	out_bookmarks_txt.close();
}

void V2::World::verifyCountriesWritten() const
{
	std::ifstream v2CountriesInput;
	v2CountriesInput.open(("output/" + theConfiguration.getOutputName() + "/common/countries.txt").c_str());
	if (!v2CountriesInput.is_open())
		throw std::runtime_error("Could not open countries.txt");

	while (!v2CountriesInput.eof())
	{
		std::string line;
		getline(v2CountriesInput, line);

		if (line[0] == '#' || line.size() < 3)
			continue;
		if (line.substr(0, 12) == "dynamic_tags")
			continue;

		std::string countryFileName;
		const auto start = line.find_first_of('/');
		const auto size = line.find_last_of('\"') - start - 1;
		countryFileName = line.substr(start + 1, size);

		if (commonItems::DoesFileExist("output/" + theConfiguration.getOutputName() + "/common/countries/" + countryFileName))
		{
			continue;
		}
		if (commonItems::DoesFileExist(theConfiguration.getVic2Path() + "/common/countries/" + countryFileName))
		{
			continue;
		}
		LOG(LogLevel::Warning) << "common/countries/" << countryFileName << " does not exists. This will likely crash Victoria 2.";
	}
	v2CountriesInput.close();
}

void V2::World::createModFile() const
{
	std::ofstream output("output/" + theConfiguration.getOutputName() + ".mod");
	if (!output.is_open())
		throw std::runtime_error("Could not create " + theConfiguration.getOutputName() + ".mod");
	LOG(LogLevel::Info) << "\t-> Writing to: "
							  << "output/" + theConfiguration.getOutputName() + ".mod";
	output << modFile;
	output.close();
}

void V2::World::outputPops() const
{
	for (const auto& popRegion: popRegions)
	{
		std::ofstream popsFile;
		popsFile.open("output/" + theConfiguration.getOutputName() + "/history/pops/1836.1.1/" + popRegion.first);
		if (!popsFile.is_open())
		{
			throw std::runtime_error("Could not create pops file output/" + theConfiguration.getOutputName() + "/history/pops/1836.1.1/" + popRegion.first);
		}

		for (auto provinceNumber: popRegion.second)
		{
			const auto& provItr = provinces.find(provinceNumber);
			if (provItr != provinces.end())
			{
				popsFile << provItr->second->getPopsForOutput();
			}
			else
			{
				LOG(LogLevel::Error) << "Could not find province " << provinceNumber << " while outputing pops!";
			}
		}
	}
}

std::shared_ptr<V2::Province> V2::World::getProvince(const int provID) const
{
	const auto& provinceItr = provinces.find(provID);
	return provinceItr != provinces.end() ? provinceItr->second : nullptr;
}

std::shared_ptr<V2::Country> V2::World::getCountry(const std::string& tag) const
{
	const auto& countryItr = countries.find(tag);
	return (countryItr != countries.end()) ? countryItr->second : nullptr;
}

std::string V2::World::clipCountryFileName(const std::string& incoming) const
{
	if (incoming.size() <= 80)
		return incoming;
	else
		return incoming.substr(0, 76) + ".txt";
}

void V2::World::copyHpmFiles() const
{
	LOG(LogLevel::Info) << "<- Copying HPM files";
	const auto& hpm = theConfiguration.getVic2Path();
	const auto& out = "output/" + theConfiguration.getOutputName();

	commonItems::CopyFolder(hpm + "/map", out + "/map");

	// gfx/interface
	commonItems::CopyFolder(hpm + "/gfx/interface/leaders", out + "/gfx/interface/leaders");
	const auto& gfxInterfaceFiles = commonItems::GetAllFilesInFolder(hpm + "/gfx/interface");
	for (const auto& file: gfxInterfaceFiles)
	{
		if (file == "icon_religion.dds")
			continue;
		fs::copy_file(hpm + "/gfx/interface/" + file, out + "/gfx/interface/" + file);
	}

	// gfx/anims
	commonItems::CopyFolder(hpm + "/gfx/anims", out + "/gfx/anims");

	// interface
	const auto& interfaceFiles = commonItems::GetAllFilesInFolder(hpm + "/interface");
	for (const auto& file: interfaceFiles)
	{
		if (file == "general_gfx.gfx")
			continue;
		fs::copy_file(hpm + "/interface/" + file, out + "/interface/" + file);
	}

	commonItems::CopyFolder(hpm + "/localisation", out + "/localisation");

	// technologies & inventions
	commonItems::DeleteFolder(out + "/technologies");
	commonItems::CopyFolder("configurables/HPM/technologies", out + "/technologies");
	commonItems::CopyFolder(hpm + "/inventions", out + "/inventions");
	commonItems::CopyFolder(hpm + "/gfx/pictures/tech", out + "/gfx/pictures/tech");

	// events & decisions
	for (const auto& file: commonItems::GetAllFilesInFolder(hpm + "/events"))
	{
		fs::remove(out + "/events/" + file);
		fs::copy_file(hpm + "/events/" + file, out + "/events/" + file);
	}
	for (const auto& file: commonItems::GetAllFilesInFolder("configurables/HPM/events"))
	{
		fs::remove(out + "/events/" + file);
		fs::copy_file("configurables/HPM/events/" + file, out + "/events/" + file);
	}
	for (const auto& file: commonItems::GetAllFilesInFolder(hpm + "/decisions"))
	{
		fs::remove(out + "/decisions/" + file);
		fs::copy_file(hpm + "/decisions/" + file, out + "/decisions/" + file);
	}
	for (const auto& file: commonItems::GetAllFilesInFolder("configurables/HPM/decisions"))
	{
		fs::remove(out + "/decisions/" + file);
		fs::copy_file("configurables/HPM/decisions/" + file, out + "/decisions/" + file);
	}

	// common
	for (const auto& file: commonItems::GetAllFilesInFolder(hpm + "/common"))
	{
		if (file == "cb_types.txt" || file == "rebel_types.txt")
			fs::remove(out + "/common/" + file);
		else if (commonItems::DoesFileExist(out + "/common/" + file))
			continue;
		fs::copy_file(hpm + "/common/" + file, out + "/common/" + file);
	}

	commonItems::CopyFolder(hpm + "/battleplans", out + "/battleplans");
	commonItems::CopyFolder(hpm + "/poptypes", out + "/poptypes");
	commonItems::CopyFolder(hpm + "/units", out + "/units");

	// news
	const auto& newsFiles = commonItems::GetAllFilesInFolder(hpm + "/news");
	for (const auto& file: newsFiles)
	{
		if (commonItems::DoesFileExist(out + "/news/" + file))
			continue;
		fs::copy_file(hpm + "/news/" + file, out + "/news/" + file);
	}

	// flag for vic2tohoi4
	std::ofstream flagFile("output/" + theConfiguration.getOutputName() + "/hybridization.txt");
	if (!flagFile.is_open())
	{
		throw std::runtime_error("Could not write hybridization file to output/" + theConfiguration.getOutputName() + "/hybridization.txt - " +
										 commonItems::GetLastErrorString());
	}
	flagFile << "HPM";
	flagFile.close();
}

void V2::World::updateCountryDetails()
{
	for (const auto& [tag, country]: countries)
	{
		for (const auto& party: country->getParties()) // load parties from countryDetails
		{
			if (!party.getPolicies().contains("social_policy"))
			{
				if (const auto& partyType = partyTypeMapper.getPartyTypeByIdeology(party.getIdeology()); partyType)
				{
					const auto& defaultPosition = partyType->getPolicyPosition("social_policy");
					country->addPolicy(party.getName(), "social_policy", defaultPosition);
				}
			}
		}
	}
}