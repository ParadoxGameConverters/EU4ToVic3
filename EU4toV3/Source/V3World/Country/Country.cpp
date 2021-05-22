#include "Country.h"
#include "../../Mappers/CountryMappings/CountryMappings.h"
#include "../../Mappers/CultureMapper/CultureMapper.h"
#include "../../Mappers/GovernmentMapper/GovernmentMapper.h"
#include "../../Mappers/LeaderTraits/LeaderTraitMapper.h"
#include "../../Mappers/PartyNames/PartyNameMapper.h"
#include "../../Mappers/PartyTypes/PartyTypeMapper.h"
#include "../../Mappers/RegimentCosts/RegimentCostsMapper.h"
#include "../../Mappers/ReligionMapper/ReligionMapper.h"
#include "../../Mappers/StartingInventionMapper/StartingInventionMapper.h"
#include "../../Mappers/StartingTechMapper/StartingTechMapper.h"
#include "../../Mappers/TechGroups/TechGroupsMapper.h"
#include "../../Mappers/TechSchools/TechSchoolMapper.h"
#include "../../Mappers/Unreleasables/Unreleasables.h"
#include "../Flags/Flags.h"
#include "CommonFunctions.h"
#include "Configuration.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include <cmath>

V2::Country::Country(const std::string& countriesFileLine,
	 const bool _dynamicCountry,
	 const mappers::PartyNameMapper& partyNameMapper,
	 const mappers::PartyTypeMapper& partyTypeMapper):
	 dynamicCountry(_dynamicCountry)
{
	// Load from a country file, if one exists. Otherwise rely on defaults.
	auto start = countriesFileLine.find_first_of('/');
	start++;
	const auto size = countriesFileLine.find_last_of('\"') - start;
	const auto filename = countriesFileLine.substr(start, size);
	details = CountryDetails(filename);
	tag = countriesFileLine.substr(0, 3);
	commonCountryFile = Localisation::convert(filename);
	modCommons = ModCommons(filename);
	initParties(partyNameMapper, partyTypeMapper);
}

V2::Country::Country(std::string _tag,
	 const std::string& _commonCountryFile,
	 const mappers::PartyNameMapper& partyNameMapper,
	 const mappers::PartyTypeMapper& partyTypeMapper):
	 tag(std::move(_tag)),
	 commonCountryFile(Localisation::convert(_commonCountryFile))
{
	newCountry = true;
	dynamicCountry = false;

	initParties(partyNameMapper, partyTypeMapper);
}

void V2::Country::initParties(const mappers::PartyNameMapper& partyNameMapper, const mappers::PartyTypeMapper& partyTypeMapper)
{
	// Check mod parties first
	std::vector<Party> parties;
	if (!modCommons.getParties().empty())
		parties = modCommons.getParties();
	else
		parties = details.parties;

	// We're a new nation so no parties are specified. Grab some.
	if (parties.empty())
		loadPartiesFromBlob(partyNameMapper, partyTypeMapper);	// Can load also for mod, only names are used

	// set a default ruling party
	for (const auto& party: parties)
	{
		// We're pinging against this date only to protect against later-game parties.
		if (party.isActiveOn(date("1836.1.1")))
		{
			// This will get overridden if this country is initialized against extant EU4 country.
			details.rulingParty = party.getName();
			break;
		}
	}
}

void V2::Country::loadPartiesFromBlob(const mappers::PartyNameMapper& partyNameMapper, const mappers::PartyTypeMapper& partyTypeMapper)
{
	size_t ideology = 0;
	for (const auto& [partyName, partyLanguageMap]: partyNameMapper.getPartyToLanguageMap())
	{
		auto partyKey = tag + '_' + partyName;
		auto languageMap = partyLanguageMap.getLanguageToNameMap();
		auto partyType = partyTypeMapper.getPartyTypeByIdeology(partyName);
		if (!partyType)
		{
			Log(LogLevel::Warning) << "Party type " << partyName << " has no entry in party_blobs.txt!";
			continue;
		}
		partyType->setName(partyKey);
		Party newParty(*partyType);
		details.parties.push_back(newParty);
		localisation.setPartyKey(ideology, partyKey);

		for (const auto& [language, name]: languageMap)
			localisation.setPartyName(ideology, language, name);

		++ideology;
	}
}

void V2::Country::initFromEU4Country(const EU4::Regions& eu4Regions,
	 std::shared_ptr<EU4::Country> _srcCountry,
	 const mappers::TechSchoolMapper& techSchoolMapper,
	 const mappers::CultureMapper& cultureMapper,
	 const mappers::IdeaEffectMapper& ideaEffectMapper,
	 const mappers::ReligionMapper& religionMapper,
	 const mappers::ProvinceMapper& provinceMapper,
	 const mappers::GovernmentMapper& governmentMapper,
	 const mappers::CountryMappings& countryMapper)
{
	srcCountry = _srcCountry;

	if (!srcCountry->getRandomName().empty())
		newCountry = true;

	auto possibleFilename = getFileFromTag("./blankMod/output/history/countries/", tag);
	if (!possibleFilename)
	{
		possibleFilename = getFileFromTag(theConfiguration.getVic2Path() + "/history/countries/", tag);
	}

	if (!possibleFilename)
	{
		auto countryName = commonCountryFile;
		countryName = trimPath(countryName);
		details.filename = tag + " - " + countryName;
	}
	else
	{
		details.filename = *possibleFilename;
	}

	// Color
	nationalColors = srcCountry->getNationalColors();

	// Localization
	localisation.setTag(tag);
	localisation.readFromCountry(*srcCountry);

	// Capital
	const auto oldCapital = srcCountry->getCapital();
	auto potentialCapitals = provinceMapper.getVic2ProvinceNumbers(oldCapital);
	if (!potentialCapitals.empty())
		details.capital = *potentialCapitals.begin();

	// HRE / Emperors
	details.inHRE = srcCountry->getInHRE();
	details.holyRomanEmperor = srcCountry->getHolyRomanEmperor();
	details.celestialEmperor = srcCountry->getCelestialEmperor();

	// religion
	setReligion(_srcCountry->getReligion(), religionMapper);

	// culture - accepted cultures are irrelevant at this stage and we decided not to use eu4's sources at all.
	setPrimaryCulture(cultureMapper, eu4Regions);

	// Government
	determineGovernmentType(ideaEffectMapper, governmentMapper);

	// Apply government effects to reforms
	finalizeInvestments(ideaEffectMapper);

	//  Politics
	resolvePolitics();

	// Generate Reforms
	reforms = Reforms(details, *srcCountry);

	// Relations
	generateRelations(countryMapper);

	// Literacy and Tech school
	calculateLiteracy();
	determineTechSchool(techSchoolMapper);

	// Consciousness
	calculateConsciousness();

	// Misc
	buildCanals();
}

void V2::Country::calculateConsciousness()
{
	const auto& legitimacy = srcCountry->getLegitimacy();
	const auto& stability = srcCountry->getStability();
	const auto& autonomy = srcCountry->getAverageAutonomy();

	// Range is [1-4] for starting countries.
	details.consciousness = 2 + (0.5 + (50 - legitimacy) / 100) - stability / 3;
	details.nonstateConsciousness = details.consciousness * autonomy / 100;
}

void V2::Country::setReligion(const std::string& religion, const mappers::ReligionMapper& religionMapper)
{
	std::optional<std::string> match = religionMapper.getVic2Religion(religion);
	if (!match)
	{
		LOG(LogLevel::Warning) << "No religion mapping defined for " << religion << " (" << srcCountry->getTag() << " -> " << tag << ')';
	}
	else
	{
		details.religion = *match;
	}
}

void V2::Country::setPrimaryCulture(const mappers::CultureMapper& cultureMapper, const EU4::Regions& eu4Regions)
{
	const auto oldCapital = srcCountry->getCapital();

	// primary culture
	auto primCulture = srcCountry->getPrimaryCulture();
	if (primCulture.empty())
	{
		primCulture = "noculture";
		LOG(LogLevel::Warning) << "No primary culture for " << srcCountry->getTag()
									  << "! Using noculture. (Is this a CK2 import? Forgot to fix CK2 converter culture/religion bugs?)";
	}

	const auto& matched = cultureMapper.cultureMatch(eu4Regions, primCulture, details.religion, oldCapital, srcCountry->getTag());
	if (!matched)
	{
		LOG(LogLevel::Warning) << "No culture mapping defined for " << primCulture << " (" << srcCountry->getTag() << " -> " << tag << ')';
	}
	else
	{
		details.primaryCulture = *matched;
		details.eu4PrimaryCulture = primCulture;
	}
}

V2::NationalValue V2::Country::getNationalValueScores() const
{
	NationalValue investments;
	investments.order = details.nationalValues.order;
	investments.liberty = details.nationalValues.liberty;
	investments.equality = details.nationalValues.equality;
	return investments;
}

void V2::Country::addRelation(Relation& newRelation)
{
	relations.insert(std::make_pair(newRelation.getTarget(), newRelation));
}

V2::Relation& V2::Country::getRelation(const std::string& target)
{
	const auto& relation = relations.find(target);
	if (relation != relations.end())
		return relation->second;
	Relation newRelation(target);
	relations.insert(std::make_pair(target, newRelation));
	const auto& newRelRef = relations.find(target);
	return newRelRef->second;
}

void V2::Country::absorbColony(Country& vassal)
{
	// change province ownership and drop provinces to colony status.
	// We are NOT dropping colonial nation cores, so the player may release the nation manually if desired.
	for (auto& province: vassal.getProvinces())
	{
		province.second->setOwner(tag);
		province.second->setController(tag);
		if (!province.second->isColony())
			province.second->setColonial(2);
		province.second->setTerritorialCore(true);
	}
	vassal.provinces.clear();

	// take vassal's armies
	srcCountry->takeArmies(vassal.getSourceCountry());

	// assume the vassal's decisions (just canals, at the moment)
	for (const auto& decision: vassal.decisions)
		decisions.insert(decision);
	vassal.decisions.clear();
}

void V2::Country::determineGovernmentType(const mappers::IdeaEffectMapper& ideaEffectMapper, const mappers::GovernmentMapper& governmentMapper)
{
	const auto& possibleGovernment = governmentMapper.matchGovernment(srcCountry->getGovernment());
	if (possibleGovernment)
		details.government = *possibleGovernment;

	for (const auto& reformStr: srcCountry->getReforms())
	{
		auto enforce = ideaEffectMapper.getEnforceFromIdea(reformStr);
		if (enforce)
			details.government = *enforce;
	}

	// Almost but not quite. Hardcoded but hey.
	if (srcCountry->isRevolutionary())
	{
		details.government = "bourgeois_dictatorship";
	}
}

void V2::Country::finalizeInvestments(const mappers::IdeaEffectMapper& ideaEffectMapper)
{
	details.technologies.army = srcCountry->getArmy();
	details.technologies.navy = srcCountry->getNavy();
	details.technologies.commerce = srcCountry->getCommerce();
	details.technologies.industry = srcCountry->getIndustry();
	details.technologies.culture = srcCountry->getCulture();
	details.reforms.slavery = srcCountry->getSlavery();
	details.reforms.upper_house_composition = srcCountry->getUpper_house_composition();
	details.reforms.vote_franchise = srcCountry->getVote_franchise();
	details.reforms.voting_system = srcCountry->getVoting_system();
	details.reforms.public_meetings = srcCountry->getPublic_meetings();
	details.reforms.press_rights = srcCountry->getPress_rights();
	details.reforms.trade_unions = srcCountry->getTrade_unions();
	details.reforms.political_parties = srcCountry->getPolitical_parties();
	details.nationalValues.liberty = srcCountry->getLiberty();
	details.nationalValues.equality = srcCountry->getEquality();
	details.nationalValues.order = srcCountry->getOrder();
	details.literacy = srcCountry->getLiteracy();
	details.upperHouses.reactionary = srcCountry->getReactionary();
	details.upperHouses.liberal = srcCountry->getLiberal();

	// Collect and finalize all idea/reform/government effects. We have combined reforms + ideas incoming, but lack government component (the last 33%)
	// Resulting scores for all of these will be between -5 and 5, with 0 being average and ignored.
	// Each point above or below 5 should alter absolute values by 10%.

	if (ideaEffectMapper.getArmyFromIdea(details.government))
		details.technologies.army = (2 * details.technologies.army + *ideaEffectMapper.getArmyFromIdea(details.government)) / 3;
	if (ideaEffectMapper.getNavyFromIdea(details.government))
		details.technologies.navy = (2 * details.technologies.navy + *ideaEffectMapper.getNavyFromIdea(details.government)) / 3;
	if (ideaEffectMapper.getCommerceFromIdea(details.government))
		details.technologies.commerce = (2 * details.technologies.commerce + *ideaEffectMapper.getCommerceFromIdea(details.government)) / 3;
	if (ideaEffectMapper.getIndustryFromIdea(details.government))
		details.technologies.industry = (2 * details.technologies.industry + *ideaEffectMapper.getIndustryFromIdea(details.government)) / 3;
	if (ideaEffectMapper.getCultureFromIdea(details.government))
		details.technologies.culture = (2 * details.technologies.culture + *ideaEffectMapper.getCultureFromIdea(details.government)) / 3;

	if (ideaEffectMapper.getSlaveryFromIdea(details.government))
		details.reforms.slavery = (2 * details.reforms.slavery + *ideaEffectMapper.getSlaveryFromIdea(details.government)) / 3;
	if (ideaEffectMapper.getUpper_house_compositionFromIdea(details.government))
		details.reforms.upper_house_composition =
			 (2 * details.reforms.upper_house_composition + *ideaEffectMapper.getUpper_house_compositionFromIdea(details.government)) / 3;
	if (ideaEffectMapper.getVote_franchiseFromIdea(details.government))
		details.reforms.vote_franchise = (2 * details.reforms.vote_franchise + *ideaEffectMapper.getVote_franchiseFromIdea(details.government)) / 3;
	if (ideaEffectMapper.getVoting_systemFromIdea(details.government))
		details.reforms.voting_system = (2 * details.reforms.voting_system + *ideaEffectMapper.getVoting_systemFromIdea(details.government)) / 3;
	if (ideaEffectMapper.getPublic_meetingsFromIdea(details.government))
		details.reforms.public_meetings = (2 * details.reforms.public_meetings + *ideaEffectMapper.getPublic_meetingsFromIdea(details.government)) / 3;
	if (ideaEffectMapper.getPress_rightsFromIdea(details.government))
		details.reforms.press_rights = (2 * details.reforms.press_rights + *ideaEffectMapper.getPress_rightsFromIdea(details.government)) / 3;
	if (ideaEffectMapper.getTrade_unionsFromIdea(details.government))
		details.reforms.trade_unions = (2 * details.reforms.trade_unions + *ideaEffectMapper.getTrade_unionsFromIdea(details.government)) / 3;
	if (ideaEffectMapper.getPolitical_partiesFromIdea(details.government))
		details.reforms.political_parties = (2 * details.reforms.political_parties + *ideaEffectMapper.getPolitical_partiesFromIdea(details.government)) / 3;

	if (ideaEffectMapper.getLibertyFromIdea(details.government))
		details.nationalValues.liberty = (2 * details.nationalValues.liberty + *ideaEffectMapper.getLibertyFromIdea(details.government)) / 3;
	if (ideaEffectMapper.getEqualityFromIdea(details.government))
		details.nationalValues.equality = (2 * details.nationalValues.equality + *ideaEffectMapper.getEqualityFromIdea(details.government)) / 3;
	if (ideaEffectMapper.getOrderFromIdea(details.government))
		details.nationalValues.order = (2 * details.nationalValues.order + *ideaEffectMapper.getOrderFromIdea(details.government)) / 3;

	if (ideaEffectMapper.getReactionaryFromIdea(details.government))
		details.upperHouses.reactionary = (2 * details.upperHouses.reactionary + *ideaEffectMapper.getReactionaryFromIdea(details.government)) / 3;
	if (ideaEffectMapper.getLiberalFromIdea(details.government))
		details.upperHouses.liberal = (2 * details.upperHouses.liberal + *ideaEffectMapper.getLiberalFromIdea(details.government)) / 3;

	if (ideaEffectMapper.getLiteracyFromIdea(details.government))
		details.literacyInvestment = (2 * details.literacyInvestment + *ideaEffectMapper.getLiteracyFromIdea(details.government)) / 3;

	// Finally a patch for those silly democracies that go too low.
	if (details.government == "democracy" && details.reforms.vote_franchise < -2.5)
		details.reforms.vote_franchise = -2;
	if (details.government == "democracy" && details.reforms.upper_house_composition < 0)
		details.reforms.upper_house_composition = 1;
}

void V2::Country::resolvePolitics()
{
	details.upperHouseReactionary = static_cast<int>(5 * (1 + details.upperHouses.reactionary * 20 / 100));
	details.upperHouseLiberal = static_cast<int>(10 * (1 + details.upperHouses.liberal * 20 / 100));
	details.upperHouseConservative = 100 - (details.upperHouseReactionary + details.upperHouseLiberal);

	if (srcCountry->isRevolutionary())
	{
		details.upperHouseReactionary = static_cast<int>(details.upperHouseReactionary / 3);
		details.upperHouseLiberal = static_cast<int>(details.upperHouseLiberal * 3);
		details.upperHouseConservative = 100 - (details.upperHouseReactionary + details.upperHouseLiberal);
	}

	std::string ideology;

	auto liberalEffect = details.upperHouses.liberal;
	const auto reactionaryEffect = details.upperHouses.reactionary;

	if (srcCountry->isRevolutionary())
	{
		liberalEffect += 10;
	}

	if (liberalEffect >= 2 * reactionaryEffect)
	{
		ideology = "liberal";
		details.upperHouseLiberal = static_cast<int>(details.upperHouseLiberal * 1.1);
		details.upperHouseConservative = 100 - (details.upperHouseReactionary + details.upperHouseLiberal);
	}
	else if (reactionaryEffect >= 2 * liberalEffect)
	{
		ideology = "reactionary";
		details.upperHouseReactionary = static_cast<int>(details.upperHouseReactionary * 1.1);
		details.upperHouseConservative = 100 - (details.upperHouseReactionary + details.upperHouseLiberal);
	}
	else
	{
		ideology = "conservative";
	}

	std::vector<Party> parties;
	if (!modCommons.getParties().empty())
		parties = modCommons.getParties();
	else
		parties = details.parties;

	for (const auto& party: parties)
	{
		if (party.isActiveOn(date("1836.1.1")) && party.getIdeology() == ideology)
		{
			details.rulingParty = party.getName();
			break;
		}
	}
}

void V2::Country::generateRelations(const mappers::CountryMappings& countryMapper)
{
	for (auto srcRelation: srcCountry->getRelations())
	{
		const auto& V2Tag = countryMapper.getV2Tag(srcRelation.first);
		if (V2Tag)
		{
			Relation newRelations(*V2Tag, srcRelation.second);
			relations.insert(std::make_pair(*V2Tag, newRelations));
		}
	}
}

void V2::Country::calculateLiteracy()
{
	details.literacy = 0.4;

	if (srcCountry->getReligion() == "protestant" || srcCountry->getReligion() == "anglican" || srcCountry->getReligion() == "confucian" ||
		 srcCountry->getReligion() == "reformed")
	{
		details.literacy += 0.1;
	}

	if (srcCountry->hasModifier("the_school_establishment_act"))
		details.literacy += 0.05;
	if (srcCountry->hasModifier("sunday_schools"))
		details.literacy += 0.05;
	if (srcCountry->hasModifier("the_education_act"))
		details.literacy += 0.05;
	if (srcCountry->hasModifier("monastic_education_system"))
		details.literacy += 0.05;
	if (srcCountry->hasModifier("western_embassy_mission"))
		details.literacy += 0.05;

	// Universities grant at most 10% literacy, with either having 10 or when having them in 10% of provinces, whichever comes sooner.
	// Colleges do half of what universities do.

	const auto& provinces = srcCountry->getProvinces();
	const auto numProvinces = provinces.size();
	auto numColleges = 0;
	auto numUniversities = 0;

	for (const auto& province: provinces)
	{
		if (province->hasBuilding("college"))
			numColleges++;
		if (province->hasBuilding("university"))
			numUniversities++;
	}

	double collegeBonus1 = 0;
	double universityBonus1 = 0;
	if (numProvinces > 0)
	{
		collegeBonus1 = static_cast<double>(numColleges) / numProvinces;
		universityBonus1 = static_cast<double>(numUniversities) * 2 / numProvinces;
	}
	const auto collegeBonus2 = numColleges * 0.005;
	const auto universityBonus2 = numUniversities * 0.01;

	const auto collegeBonus = std::min(std::max(collegeBonus1, collegeBonus2), 0.05);
	const auto universityBonus = std::min(std::max(universityBonus1, universityBonus2), 0.1);

	details.literacy += collegeBonus + universityBonus;

	if (details.literacy > theConfiguration.getMaxLiteracy())
	{
		details.literacy = theConfiguration.getMaxLiteracy();
	}

	// Finally apply collective national literacy modifier.

	details.literacy *= 1 + details.literacyInvestment * 10 / 100;
}

void V2::Country::determineTechSchool(const mappers::TechSchoolMapper& techSchoolMapper)
{
	details.techSchool = techSchoolMapper.findBestTechSchool(details.technologies.army,
		 details.technologies.commerce,
		 details.technologies.culture,
		 details.technologies.industry,
		 details.technologies.navy);
}

void V2::Country::buildCanals()
{
	for (const auto& prov: srcCountry->getProvinces())
	{
		if (prov->hasGreatProject("suez_canal"))
			decisions.insert("build_suez_canal");
		if (prov->hasGreatProject("kiel_canal"))
			decisions.insert("build_kiel_canal");
		if (prov->hasGreatProject("panama_canal"))
			decisions.insert("build_panama_canal");
	}
}

// used only for countries which are NOT converted (i.e. unions, dead countries, etc)
void V2::Country::initFromHistory(const mappers::Unreleasables& unreleasablesMapper,
	 const mappers::PartyNameMapper& partyNameMapper,
	 const mappers::PartyTypeMapper& partyTypeMapper)
{
	// Ping unreleasable_tags for this country's TAG
	details.isReleasableVassal = unreleasablesMapper.isTagReleasable(tag);

	// Don't fire up Details if there's no point.
	auto possibleFilename = getFileFromTag("./blankMod/output/history/countries/", tag);
	if (!possibleFilename)
		possibleFilename = getFileFromTag(theConfiguration.getVic2Path() + "/history/countries/", tag);
	if (!possibleFilename)
	{
		auto countryName = commonCountryFile;
		auto lastSlash = countryName.find_last_of('/');
		countryName = countryName.substr(++lastSlash, countryName.size());
		details.filename = tag + " - " + countryName;
		return;
	}
	details = CountryDetails(*possibleFilename);
	initParties(partyNameMapper, partyTypeMapper);
}

void V2::Country::addProvince(std::shared_ptr<Province> _province)
{
	const auto& itr = provinces.find(_province->getID());
	if (itr != provinces.end())
		LOG(LogLevel::Error) << "Inserting province " << _province->getID() << " multiple times (addProvince())";
	provinces.insert(make_pair(_province->getID(), _province));
}

void V2::Country::addState(std::shared_ptr<State> newState, const mappers::PortProvinces& portProvincesMapper)
{
	int highestNavalLevel = 0;
	unsigned int hasHighestLevel = -1;

	states.push_back(newState);
	auto newProvinces = newState->getProvinces();

	std::set<int> newProvinceNums;
	for (const auto& province: newProvinces)
	{
		newProvinceNums.insert(province->getID());
	}
	auto portProvinces = Army::getPortProvinces(newProvinceNums, provinces, portProvincesMapper);

	for (unsigned int i = 0; i < newProvinces.size(); i++)
	{
		auto itr = provinces.find(newProvinces[i]->getID());
		if (itr == provinces.end())
		{
			provinces.insert(make_pair(newProvinces[i]->getID(), newProvinces[i]));
		}

		// find the province with the highest naval base level
		const auto isPortProvince = std::find(portProvinces.begin(), portProvinces.end(), newProvinces[i]->getID()) != portProvinces.end();
		if (newProvinces[i]->getNavalBaseLevel() > highestNavalLevel && isPortProvince)
		{
			highestNavalLevel = newProvinces[i]->getNavalBaseLevel();
			hasHighestLevel = i;
		}
		newProvinces[i]->setNavalBaseLevel(0);
	}
	if (highestNavalLevel > 0)
	{
		newProvinces[hasHighestLevel]->setNavalBaseLevel(highestNavalLevel);
	}
}

void V2::Country::convertLeaders(const mappers::LeaderTraitMapper& leaderTraitMapper)
{
	if (srcCountry == nullptr)
		return;
	if (provinces.empty())
		return;
	auto eu4Leaders = srcCountry->getMilitaryLeaders();
	if (eu4Leaders.empty())
		return;

	for (const auto& eu4Leader: eu4Leaders)
	{
		if (eu4Leader.isFemale())
			continue; // Sorry. No female generals or admirals.
		Leader newLeader(eu4Leader, leaderTraitMapper);
		leaders.push_back(newLeader);
	}
}

void V2::Country::convertUncivReforms(const CIV_ALGORITHM techGroupAlgorithm,
	 const double topTech,
	 const int topInstitutions,
	 const mappers::TechGroupsMapper& techGroupsMapper)
{
	switch (techGroupAlgorithm)
	{
		case (CIV_ALGORITHM::older):
			oldCivConversionMethod();
			break;
		case (CIV_ALGORITHM::newer):
			newCivConversionMethod(topTech, topInstitutions, techGroupsMapper);
			break;
		default:
			break;
	}
}

// civilization level conversion method for games up to 1.18
void V2::Country::oldCivConversionMethod()
{
	if (!srcCountry)
		return;
	const auto totalTechs = srcCountry->getMilTech() + srcCountry->getAdmTech();
	const auto militaryDev = srcCountry->getMilTech() / totalTechs;
	const auto socioEconDev = srcCountry->getAdmTech() / totalTechs;

	if (srcCountry->getTechGroup() == "western" || srcCountry->getTechGroup() == "high_american" || srcCountry->getTechGroup() == "eastern" ||
		 srcCountry->getTechGroup() == "ottoman" || srcCountry->numEmbracedInstitutions() >= 7) // civilized, do nothing
	{
		details.civilized = true;
	}
	else if ((srcCountry->getTechGroup() == "north_american" || srcCountry->getTechGroup() == "mesoamerican" || srcCountry->getTechGroup() == "south_american" ||
					 srcCountry->getTechGroup() == "new_world" || srcCountry->getTechGroup() == "andean") &&
				srcCountry->numEmbracedInstitutions() <= 3)
	{
		uncivReforms = UncivReforms(0, militaryDev, socioEconDev, this);
	}
	else if (srcCountry->getIsolationism() == 0 && srcCountry->numEmbracedInstitutions() >= 6)
	{
		uncivReforms = UncivReforms(50, militaryDev, socioEconDev, this);
	}
	else if (srcCountry->getTechGroup() == "muslim" || srcCountry->numEmbracedInstitutions() >= 6)
	{
		uncivReforms = UncivReforms(44, militaryDev, socioEconDev, this);
	}
	else if (srcCountry->getTechGroup() == "indian" || srcCountry->getIsolationism() == 0)
	{
		uncivReforms = UncivReforms(40, militaryDev, socioEconDev, this);
	}
	else if (srcCountry->getTechGroup() == "chinese" || srcCountry->numEmbracedInstitutions() == 5)
	{
		uncivReforms = UncivReforms(36, militaryDev, socioEconDev, this);
	}
	else if (srcCountry->getTechGroup() == "nomad_group")
	{
		uncivReforms = UncivReforms(30, militaryDev, socioEconDev, this);
	}
	else if (srcCountry->getTechGroup() == "sub_saharan" || srcCountry->getTechGroup() == "central_african" || srcCountry->getTechGroup() == "east_african" ||
				srcCountry->numEmbracedInstitutions() == 4)
	{
		uncivReforms = UncivReforms(20, militaryDev, socioEconDev, this);
	}
	else
	{
		LOG(LogLevel::Warning) << "Unhandled tech group (" << srcCountry->getTechGroup() << " with " << srcCountry->numEmbracedInstitutions()
									  << " institutions) for " << tag << " - giving no reforms";
		uncivReforms = UncivReforms(0, militaryDev, socioEconDev, this);
	}
}

// civilization level conversion method for games 1.19+
void V2::Country::newCivConversionMethod(double topTech, int topInstitutions, const mappers::TechGroupsMapper& techGroupsMapper)
{
	if (!srcCountry)
		return;

	auto totalTechs = srcCountry->getMilTech() + srcCountry->getAdmTech() + srcCountry->getDipTech();

	// set civilization cut off for 6 techs behind the the tech leader (30 years behind tech)
	// set number for civilization level based on techs and institutions
	// at 31 techs behind completely uncivilized
	// each institution behind is equivalent to 2 techs behind

	auto civLevel = (totalTechs + 31.0 - topTech) * 4;
	civLevel = civLevel + (static_cast<double>(srcCountry->numEmbracedInstitutions()) - topInstitutions) * 8;
	if (civLevel > 100)
		civLevel = 100;
	if (civLevel < 0)
		civLevel = 0;

	const auto& techGroup = srcCountry->getTechGroup();

	if (theConfiguration.getEuroCentrism() == Configuration::EUROCENTRISM::EuroCentric)
	{
		// For eurocentric setups, we do some overrides. Literacy is boosted or reduced according to specs
		details.literacy *= 1 + (static_cast<double>(techGroupsMapper.getLiteracyFromTechGroup(techGroup)) - 5.0) * 10.0 / 100.0;
		// Westernization=10 countries must be civilized even if lagging behind in tech and scores, otherwise we scale it according to config file.
		if (techGroupsMapper.getWesternizationFromTechGroup(techGroup) == 10)
			civLevel = 100;
		else
			civLevel = civLevel * (static_cast<double>(techGroupsMapper.getWesternizationFromTechGroup(techGroup)) / 10.0);
	}

	// Hardcoded exception for shinto countries so japan-likes may retain high industrialization potential
	if (details.religion == "shinto")
		details.literacy *= theConfiguration.getMaxLiteracy();
	else
		details.literacy *= theConfiguration.getMaxLiteracy() * (pow(10, civLevel / 100 * 0.9 + 0.1) / 10);

	/*
	Drop nominal literacy according to starting date. The curve is crafted to hit the following literacy percentage points:
	1836: 1
	1821: 0.85
	1750: 0.5
	1650: 0.3
	1490: 0.2
	1350: 0.15
	It will fail to hit those points exactly but won't err by much.
	*/

	auto lastDate = theConfiguration.getLastEU4Date();
	if (lastDate > HARD_ENDING_DATE)
		lastDate = HARD_ENDING_DATE;

	const auto currentYear = std::fmax(lastDate.diffInYears(date("0.1.1")), 0);
	const auto yearFactor = (0.1 + 4'614'700 * currentYear) / (1 + 103'810'000.0f * currentYear - 54'029 * pow(currentYear, 2));

	details.literacy *= yearFactor;

	if (civLevel == 100)
		details.civilized = true;

	if (details.civilized == false)
	{
		totalTechs = totalTechs - srcCountry->getDipTech();
		const auto militaryDev = srcCountry->getMilTech() / totalTechs;
		const auto socioEconDev = srcCountry->getAdmTech() / totalTechs;
		uncivReforms = UncivReforms(lround(civLevel), militaryDev, socioEconDev, this);
	}
}

void V2::Country::addRailroadtoCapitalState()
{
	for (const auto& state: states)
	{
		if (state->provInState(details.capital))
		{
			state->addRailroads();
			break;
		}
	}
}

void V2::Country::convertLandlessReforms(const Country& capitalOwner) // Use current capital owner to set up.
{
	// We can only copy reforms from the same civilized-level nation. This means India might be uncivilized, but inherit britain's civilized reforms,
	// which come into play once it is released and westernizes.

	if (capitalOwner.isCivilized())
	{
		reforms = capitalOwner.getReforms();
	}
	else
	{
		uncivReforms = capitalOwner.getUncivReforms();
	}
}

void V2::Country::setTechs(const mappers::StartingTechMapper& startingTechMapper,
	 const mappers::StartingInventionMapper& startingInventionMapper,
	 double armyScore,
	 double navyScore,
	 double cultureScore,
	 double industryScore,
	 double commerceScore)
{
	if (!details.civilized)
		return;

	const auto armyTechs = startingTechMapper.getTechsForGroupAndScore("army", armyScore);
	const auto navyTechs = startingTechMapper.getTechsForGroupAndScore("navy", navyScore);
	const auto cultureTechs = startingTechMapper.getTechsForGroupAndScore("culture", cultureScore);
	const auto industryTechs = startingTechMapper.getTechsForGroupAndScore("industry", industryScore);
	const auto commerceTechs = startingTechMapper.getTechsForGroupAndScore("commerce", commerceScore);

	techs.insert(armyTechs.begin(), armyTechs.end());
	techs.insert(navyTechs.begin(), navyTechs.end());
	techs.insert(cultureTechs.begin(), cultureTechs.end());
	techs.insert(industryTechs.begin(), industryTechs.end());
	techs.insert(commerceTechs.begin(), commerceTechs.end());

	inventions = startingInventionMapper.getInventionsForTechs(techs);
}

bool V2::Country::addFactory(std::shared_ptr<Factory> factory)
{
	// check factory techs
	const auto requiredTech = factory->getRequiredTech();
	if (!requiredTech.empty())
	{
		if (!techs.count(requiredTech))
			return false;
	}

	// check factory inventions
	const auto requiredInvention = factory->getRequiredInvention();
	if (!requiredInvention.empty())
	{
		if (!inventions.count(requiredInvention))
			return false;
	}

	// find a state to add the factory to, which meets the factory's requirements
	std::vector<std::pair<double, std::shared_ptr<State>>> candidates;
	for (const auto& candidate: states)
	{
		if (candidate->isColonial())
			continue;
		if (candidate->getFactoryCount() >= 8)
			continue;
		if (factory->requiresCoastal())
			if (!candidate->isCoastal())
				continue;

		auto candidateScore = candidate->getSuppliedInputs(factory) * 100;
		candidateScore -= static_cast<double>(candidate->getFactoryCount()) * 10;
		candidateScore += candidate->getMfgRatio();
		candidates.emplace_back(std::pair(candidateScore, candidate));
	}

	sort(candidates.begin(), candidates.end(), [](const std::pair<double, std::shared_ptr<State>>& lhs, const std::pair<double, std::shared_ptr<State>>& rhs) {
		if (lhs.first != rhs.first)
			return lhs.first > rhs.first;
		return lhs.second->getID() < rhs.second->getID();
	});

	if (candidates.empty())
		return false;

	candidates[0].second->addFactory(factory);
	details.numFactories++;
	return true;
}

void V2::Country::setupPops(const double popWeightRatio, const CIV_ALGORITHM popConversionAlgorithm, const mappers::ProvinceMapper& provinceMapper)
{
	// create the pops
	for (const auto& province: provinces)
		province.second->doCreatePops(popWeightRatio, this, popConversionAlgorithm, provinceMapper);
}

void V2::Country::convertArmies(const mappers::RegimentCostsMapper& regimentCostsMapper,
	 const std::map<int, std::shared_ptr<Province>>& allProvinces,
	 const mappers::PortProvinces& portProvincesMapper,
	 const mappers::ProvinceMapper& provinceMapper)
{
	if (srcCountry == nullptr)
		return;
	if (provinces.empty())
		return;

	// set up armies with whatever regiments they deserve, rounded down
	// and keep track of the remainders for later
	for (auto& eu4Army: srcCountry->getArmies())
	{
		Army army(eu4Army,
			 tag,
			 details.civilized,
			 regimentCostsMapper,
			 allProvinces,
			 provinceMapper,
			 portProvincesMapper,
			 unitNameCount,
			 localisation.getLocalAdjective());
		if (army.success())
			armies.push_back(army); // That went well.
		// copy over remainders, if any.
		auto armyRemainders = army.getArmyRemainders();
		for (const auto& remainder: armyRemainders)
			countryRemainders[remainder.first] += remainder.second;
	}

	// allocate the remainders from the whole country to the armies according to their need, rounding up
	for (auto& remainder: countryRemainders)
	{
		while (remainder.second > 0.0)
		{
			auto army = getArmyForRemainder(remainder.first);
			if (army == nullptr)
				break;

			switch (army->addRegimentToArmy(remainder.first, allProvinces, provinceMapper, portProvincesMapper, unitNameCount, localisation.getLocalAdjective()))
			{
				case AddRegimentToArmyResult::success:
					remainder.second -= 1.0;
					army->addRegimentRemainder(remainder.first, -1.0);
					break;
				case AddRegimentToArmyResult::retry:
					break;
				case AddRegimentToArmyResult::fail:
					army->addRegimentRemainder(remainder.first, -2000.0);
					break;
			}
		}
	}
}

// find the army most in need of a regiment of this category
V2::Army* V2::Country::getArmyForRemainder(const REGIMENTTYPE chosenType)
{
	Army* retval = nullptr;
	auto retvalRemainder = -1000.0;
	for (auto& army: armies)
	{
		// only add units to armies that originally had units of the same category
		if (army.hasRegimentsOfType(chosenType))
		{
			if (army.getRegimentRemainder(chosenType) > retvalRemainder)
			{
				retvalRemainder = army.getRegimentRemainder(chosenType);
				retval = &army;
			}
		}
	}

	return retval;
}

std::string V2::Country::getColonialRegion() const
{
	if (!srcCountry)
		return std::string();
	return srcCountry->getColonialRegion();
}

std::optional<std::string> V2::Country::getFileFromTag(const std::string& directoryPath, const std::string& tag) const
{
	auto foundFiles = commonItems::GetAllFilesInFolder(directoryPath);
	for (std::string file: foundFiles)
	{
		if (tag == file.substr(0, 3))
		{
			return file;
		}
	}

	return {};
}

void V2::Country::addPolicy(const std::string& partyName, const std::string& policy, const std::string& position)
{
	if (const auto& partyItr = std::find_if(details.parties.begin(),
			  details.parties.end(),
			  [partyName](const V2::Party& party) {
				  return party.getName() == partyName;
			  });
		 partyItr != details.parties.end())
		partyItr->addPolicy(policy, position);
}

void V2::Country::setHpmNationalValues()
{
	// Take nv_equality and nv_liberty from the Converter
	if (details.nationalValue == "nv_equality" || details.nationalValue == "nv_liberty")
	{
		return;
	}

	double productionEff = 0.0;
	double tradeEff = 0.0;
	const auto& reforms = srcCountry->getReforms();
	const auto& ideas = srcCountry->getNationalIdeas();
	const auto& policies = srcCountry->getPolicies();

	// Production
	if (reforms.contains("administrative_monarchy")
		|| reforms.contains("administrative_republic")
		|| reforms.contains("constitutional_republic")
		|| reforms.contains("peasants_republic_legacy")
		|| reforms.contains("general_estates_reform")
		|| reforms.contains("peasants_republic")
		) {
		productionEff += 0.1;
	}
	if (ideas.contains("economic_ideas")) productionEff += 0.1;
	if (ideas.contains("custom_idea_production_efficiency")) productionEff += 0.05;
	if (policies.contains("the_combination_act")) productionEff += 0.2;
	if (policies.contains("enlightened_aristocracy")) productionEff += 0.1;
	if (policies.contains("the_transportation_act")) productionEff += 0.1;
	if (policies.contains("the_statute_of_monopolies")) productionEff += 0.1;
	if (policies.contains("taxation_with_representation")) productionEff += 0.1;

	// Trade
	if (reforms.contains("constitutional_republic")
		|| reforms.contains("imperial_city")
		|| reforms.contains("dutch_republic_legacy")
		|| reforms.contains("enforce_trader_privileges_reform")
		|| reforms.contains("bengali_reform")
		|| reforms.contains("free_city")
		|| reforms.contains("dutch_republic")
		) {
		tradeEff += 0.1;
	}
	if (ideas.contains("trade_ideas")) tradeEff += 0.1;
	if (ideas.contains("custom_idea_trade_efficiency")) tradeEff += 0.05;
	if (policies.contains("the_statute_of_monopolies")) tradeEff += 0.1;
	if (policies.contains("the_banking_system")) tradeEff += 0.1;
	if (policies.contains("the_importation_act")) tradeEff += 0.2;
	if (policies.contains("cloth_quality_edict")) tradeEff += 0.2;
	if (policies.contains("encouragement_of_merchant_navy")) tradeEff += 0.1;
	if (policies.contains("public_road_act")) tradeEff += 0.1;

	// nv_order
	const auto& armyTradition = srcCountry->getArmyTradition();
	const auto& navyTradition = srcCountry->getNavyTradition();
	const auto& armyProfessionalism = srcCountry->getArmyProfessionalism();
	const auto& absolutism = srcCountry->getAbsolutism();

	if (productionEff + tradeEff >= 0.6)
	{
		details.nationalValue = "nv_productivity";
	}
	else if (armyTradition > 50 || navyTradition > 75 || armyProfessionalism > 0.8)
	{
		details.nationalValue = "nv_order";
	}
	else if (absolutism > 75)
	{
		details.nationalValue = "nv_autocracy";
	}
	else
	{
		details.nationalValue = "nv_tradition";
	}
}