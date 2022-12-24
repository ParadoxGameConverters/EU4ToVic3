#include "EU4Country.h"
#include "CommonRegexes.h"
#include "CountryGovernment/EU4GovernmentSection.h"
#include "CountryHistory/CountryHistory.h"
#include "CountryLeader/EU4LeaderID.h"
#include "CountryRelations/EU4Relations.h"
#include "EU4ActiveIdeas.h"
#include "EU4ActivePolicy.h"
#include "EU4CountryFlags.h"
#include "EU4CountryModifier.h"
#include "EU4Technology.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"
#include <cmath>
#include <numeric>

EU4::Country::Country(std::string countryTag, std::istream& theStream): tag(std::move(countryTag))
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();

	// for countries lacking explicit culture/religion we grab the one in first history entry.
	if (primaryCulture.empty() && !historicalPrimaryCulture.empty())
		primaryCulture = historicalPrimaryCulture;
	if (religion.empty() && !historicalReligion.empty())
		religion = historicalReligion;

	determineJapaneseRelations();
	filterLeaders();

	// finalize history data.
	if (government == "republic" || government == "theocracy")
		historicalEntry.monarchy = false;
}

void EU4::Country::registerKeys()
{
	registerKeyword("name", [this](std::istream& theStream) {
		name = commonItems::getString(theStream);
		// this is win1252 name, so, immediately convert to UTF8.
		name = commonItems::convertWin1252ToUTF8(name);
	});
	registerKeyword("adjective", [this](std::istream& theStream) {
		adjective = commonItems::singleString(theStream).getString();
		// this is win1252 adjective, so, immediately convert to UTF8.
		adjective = commonItems::convertWin1252ToUTF8(adjective);
	});
	registerKeyword("colors", [this](std::istream& theStream) {
		nationalColors = NationalSymbol(theStream);
	});
	registerKeyword("capital", [this](std::istream& theStream) {
		capital = commonItems::getInt(theStream);
	});
	registerKeyword("technology_group", [this](std::istream& theStream) {
		techGroup = commonItems::getString(theStream);
	});
	registerKeyword("liberty_desire", [this](std::istream& theStream) {
		libertyDesire = commonItems::getDouble(theStream);
	});
	registerKeyword("institutions", [this](std::istream& theStream) {
		for (const auto institution: commonItems::getInts(theStream))
			if (institution == 1)
				embracedInstitutions.push_back(true);
			else
				embracedInstitutions.push_back(false);
	});
	registerKeyword("isolationism", [this](std::istream& theStream) {
		isolationism = commonItems::getInt(theStream);
	});
	registerKeyword("primary_culture", [this](std::istream& theStream) {
		primaryCulture = commonItems::getString(theStream);
	});
	registerKeyword("religion", [this](std::istream& theStream) {
		religion = commonItems::getString(theStream);
	});
	registerKeyword("age_score", [this](std::istream& theStream) {
		for (const auto& agScore: commonItems::getDoubles(theStream))
			score += agScore;
	});
	registerKeyword("stability", [this](std::istream& theStream) {
		stability = commonItems::singleDouble(theStream).getDouble();
	});
	registerKeyword("technology", [this](std::istream& theStream) {
		const EU4Technology techBlock(theStream);
		admTech = techBlock.getAdm();
		dipTech = techBlock.getDip();
		milTech = techBlock.getMil();
	});
	registerRegex("flags|hidden_flags|variables", [this](const std::string& unused, std::istream& theStream) {
		auto flagsList = EU4CountryFlags(theStream).getFlags();
		flags.insert(flagsList.begin(), flagsList.end());
	});
	registerKeyword("modifier", [this](std::istream& theStream) {
		const EU4CountryModifier newModifier(theStream);
		if (!newModifier.getModifier().empty())
			modifiers.insert(newModifier.getModifier());
	});
	registerKeyword("government", [this](std::istream& theStream) {
		const GovernmentSection theSection(theStream);
		government = theSection.getGovernment();
		governmentReforms = theSection.getGovernmentReforms();
	});
	registerKeyword("government_rank", [this](std::istream& theStream) {
		governmentRank = commonItems::getInt(theStream);
	});
	registerKeyword("active_relations", [this](std::istream& theStream) {
		const EU4Relations activeRelations(theStream);
		relations = activeRelations.getRelations();
	});
	registerRegex("army|navy", [this](const std::string& armyFloats, std::istream& theStream) {
		const EU4Army theArmy(theStream, armyFloats);
		armies.push_back(theArmy);
	});
	registerKeyword("active_idea_groups", [this](std::istream& theStream) {
		const EU4ActiveIdeas activeIdeas(theStream);
		nationalIdeas = activeIdeas.getActiveIdeas();
	});
	registerRegex("legitimacy|horde_unity|devotion|meritocracy|republican_tradition", [this](const std::string& unused, std::istream& theStream) {
		legitimacyEquivalent = commonItems::getDouble(theStream);
	});
	registerKeyword("average_autonomy", [this](std::istream& theStream) {
		averageAutonomy = commonItems::singleDouble(theStream).getDouble();
	});
	registerKeyword("colonial_parent", [this](std::istream& theStream) {
		commonItems::ignoreItem("unused", theStream);
		colony = true;
	});
	registerKeyword("overlord", [this](std::istream& theStream) {
		overlord = commonItems::getString(theStream);
	});
	registerKeyword("history", [this](std::istream& theStream) {
		const CountryHistory theCountryHistory(theStream);
		historicalLeaders = theCountryHistory.getLeaders();
		if (!theCountryHistory.getDynasty().empty())
			historicalEntry.lastDynasty = theCountryHistory.getDynasty();
		historicalPrimaryCulture = theCountryHistory.getPrimaryCulture();
		historicalReligion = theCountryHistory.getReligion();
	});
	registerKeyword("leader", [this](std::istream& theStream) {
		const LeaderID idBlock(theStream);
		activeLeaderIDs.insert(idBlock.getIDNum());
	});
	registerKeyword("active_policy", [this](std::istream& theStream) {
		const EU4ActivePolicy policyBlock(theStream);
		policies.insert(policyBlock.getPolicy());
	});
	registerKeyword("absolutism", [this](std::istream& theStream) {
		absolutism = commonItems::getDouble(theStream);
	});
	registerKeyword("army_tradition", [this](std::istream& theStream) {
		armyTradition = commonItems::getDouble(theStream);
	});
	registerKeyword("navy_tradition", [this](std::istream& theStream) {
		navyTradition = commonItems::getDouble(theStream);
	});
	registerKeyword("army_professionalism", [this](std::istream& theStream) {
		armyProfessionalism = commonItems::getDouble(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

void EU4::Country::filterLeaders()
{
	// Dropping all leaders not currently in service (regardless of assignment).
	for (const auto& leader: historicalLeaders)
		if (activeLeaderIDs.find(leader.getID()) != activeLeaderIDs.end())
			militaryLeaders.push_back(leader);
}

void EU4::Country::determineJapaneseRelations()
{
	if (governmentReforms.contains("shogunate"))
		possibleShogun = true;
	if (governmentReforms.contains("indep_daimyo"))
		possibleDaimyo = true;
	if (governmentReforms.contains("daimyo"))
		possibleDaimyo = true;
}

void EU4::Country::setLocalizationName(const std::string& language, const std::string& incName)
{
	namesByLanguage[language] = incName;
	if (name.empty() && language == "english")
		name = incName;
}

void EU4::Country::setLocalizationAdjective(const std::string& language, const std::string& incAdjective)
{
	adjectivesByLanguage[language] = incAdjective;
	if (adjective.empty() && language == "english")
		adjective = incAdjective;
}

void EU4::Country::addProvince(const std::shared_ptr<Province>& province)
{
	provinces.push_back(province);
}

void EU4::Country::addCore(const std::shared_ptr<Province>& core)
{
	cores.push_back(core);
}

bool EU4::Country::hasModifier(const std::string& modifier) const
{
	return modifiers.contains(modifier);
}

bool EU4::Country::hasNationalIdea(const std::string& ni) const
{
	return nationalIdeas.contains(ni);
}

bool EU4::Country::hasFlag(const std::string& flag) const
{
	return flags.contains(flag);
}

bool EU4::Country::hasReform(const std::string& reform) const
{
	return governmentReforms.contains(reform);
}

void EU4::Country::updateRegimentTypes(const UnitTypeLoader& unitTypeLoader)
{
	for (auto& theArmy: armies)
		theArmy.updateRegimentTypes(unitTypeLoader);
}

void EU4::Country::buildManufactoryCount(const std::map<std::string, std::shared_ptr<Country>>& theCountries)
{
	for (const auto& province: provinces)
	{
		if (province->hasBuilding("weapons"))
			++mfgCount;
		if (province->hasBuilding("wharf"))
			++mfgCount;
		if (province->hasBuilding("textile"))
			++mfgCount;
		if (province->hasBuilding("plantations"))
			++mfgCount;
		if (province->hasBuilding("tradecompany"))
			++mfgCount;
		if (province->hasBuilding("farm_estate"))
			++mfgCount;
		if (province->hasBuilding("mills"))
			++mfgCount;
		if (province->hasBuilding("furnace"))
			mfgCount += 3;
	}
	if (!overlord.empty())
	{
		const auto transferCount = mfgCount - lround(mfgCount / 2);
		mfgTransfer -= transferCount;
		const auto& overlordCountry = theCountries.find(overlord);
		if (overlordCountry != theCountries.end())
			overlordCountry->second->increaseMfgTransfer(transferCount);
	}
}

double EU4::Country::getManufactoryDensity() const
{
	if (provinces.empty())
		return 0;
	else
		return static_cast<double>(mfgCount) / static_cast<double>(provinces.size());
}

void EU4::Country::eatCountry(const std::shared_ptr<Country>& target)
{
	// auto-cannibalism is forbidden
	if (target->getTag() == tag)
		return;

	Log(LogLevel::Info) << " - " << tag << " is assimilating " << target->getTag();

	// for calculation of weighted averages
	auto totalProvinces = static_cast<double>(target->provinces.size() + provinces.size());
	if (totalProvinces == 0.0)
		totalProvinces = 1;

	const auto myWeight = static_cast<double>(provinces.size()) / totalProvinces;
	const auto targetWeight = static_cast<double>(target->provinces.size()) / totalProvinces;

	// acquire target's cores (always)
	for (const auto& core: target->getCores())
	{
		addCore(core);
		core->addCore(tag);
		core->removeCore(target->tag);
	}

	// everything else, do only if this country actually currently exists
	if (!target->provinces.empty())
	{
		// acquire target's provinces
		for (const auto& province: target->provinces)
		{
			province->setOwnerTag(tag);
			province->setControllerTag(tag);
			addProvince(province);
		}

		// acquire target's armies, navies, admirals, and generals
		takeArmies(target);

		// re-balance prestige and techs from weighted average
		score = myWeight * score + targetWeight * target->score;
		admTech = myWeight * admTech + targetWeight * target->admTech;
		dipTech = myWeight * dipTech + targetWeight * target->dipTech;
		milTech = myWeight * milTech + targetWeight * target->milTech;
	}

	// coreless, landless countries will be cleaned up automatically
	target->clearProvinces();
	target->clearCores();
}

void EU4::Country::takeArmies(const std::shared_ptr<Country>& target)
{
	// acquire target's armies, navies, admirals, and generals
	armies.insert(armies.end(), target->armies.begin(), target->armies.end());
	militaryLeaders.insert(militaryLeaders.end(), target->militaryLeaders.begin(), target->militaryLeaders.end());
	target->clearArmies();
}

void EU4::Country::clearArmies()
{
	armies.clear();
	militaryLeaders.clear();
}

bool EU4::Country::cultureSurvivesInCores(const std::map<std::string, std::shared_ptr<Country>>& theCountries) const
{
	// If this were a LANDLESS country, is there some other country that contains our people?
	// If so, we shouldn't delete this country as it may reappear.

	for (const auto& core: cores)
	{
		if (core->getOwnerTag().empty()) // this core province belongs to noone.
			continue;
		if (core->getCulturePercent(primaryCulture) < 0.5) // this core province doesn't have our people.
			continue;

		auto owner = theCountries.find(core->getOwnerTag());												  // ownerstring will be someone else, not us.
		if (owner != theCountries.end() && owner->second->getPrimaryCulture() != primaryCulture) // and they are not us, ethnically.
			return true;
	}
	return false;
}

std::string EU4::Country::getName(const std::string& language) const
{
	if (namesByLanguage.contains(language))
		return namesByLanguage.at(language);

	// if we're lacking a specific language, try with english.
	if (namesByLanguage.contains("english"))
		return namesByLanguage.at("english");

	// otherwise, eh.
	return name;
}

std::string EU4::Country::getAdjective(const std::string& language) const
{
	if (adjectivesByLanguage.contains(language))
		return adjectivesByLanguage.at(language);

	if (adjectivesByLanguage.contains("english"))
		return adjectivesByLanguage.at("english");

	return adjective;
}

int EU4::Country::getNumEmbracedInstitutions() const
{
	auto total = 0;
	for (const auto institution: embracedInstitutions)
		if (institution)
			total++;
	return total;
}

void EU4::Country::clearProvinces()
{
	provinces.clear();
}

void EU4::Country::clearCores()
{
	cores.clear();
}

double EU4::Country::getCountryWeight() const
{
	auto totalDev = 0.0;
	for (const auto& province: provinces)
		totalDev += province->getProvinceWeight();
	return totalDev;
}

double EU4::Country::getAverageDevelopment() const
{
	if (provinces.empty())
		return 0;
	const double totalDev = std::accumulate(provinces.begin(), provinces.end(), 0, [](double sum, const std::shared_ptr<Province>& province) {
		return sum + province->getBaseTax() + province->getBaseProduction() + province->getBaseManpower();
	});
	return totalDev / static_cast<double>(provinces.size());
}
