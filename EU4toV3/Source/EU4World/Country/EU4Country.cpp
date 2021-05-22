#include "EU4Country.h"
#include "Configuration.h"
#include "EU4ActiveIdeas.h"
#include "EU4ActivePolicy.h"
#include "EU4CountryFlags.h"
#include "EU4CountryModifier.h"
#include "EU4GovernmentSection.h"
#include "EU4Technology.h"
#include "History/CountryHistory.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"
#include "Relations/EU4Relations.h"
#include "V3World/Localisation/Localisation.h"
#include <cmath>
#include "CommonRegexes.h"
#include "Leader/EU4LeaderID.h"

EU4::Country::Country(std::string countryTag, const GameVersion& theVersion, std::istream& theStream, const mappers::IdeaEffectMapper& ideaEffectMapper):
	 tag(std::move(countryTag))
{
	registerKeys(theVersion);
	parseStream(theStream);
	clearRegisteredKeywords();

	// for countries lacking explicit culture/religion we grab the one in first history entry.
	if (primaryCulture.empty() && !historicalPrimaryCulture.empty())
		primaryCulture = historicalPrimaryCulture;
	if (religion.empty() && !historicalReligion.empty())
		religion = historicalReligion;

	// since custom_name was removed now we have to resort to this...
	if (tag[0] == 'D' && isdigit(tag[1]) && isdigit(tag[2]))
		customNation = true;

	determineJapaneseRelations();
	determineInvestments(ideaEffectMapper);
	determineLibertyDesire();
	filterLeaders();

	// finalize history data.
	if (government == "republic" || government == "theocracy")
		historicalEntry.monarchy = false;
}

void EU4::Country::registerKeys(const GameVersion& theVersion)
{
	registerKeyword("name", [this](const std::string& unused, std::istream& theStream) {
		name = commonItems::singleString(theStream).getString();
		name = commonItems::normalizeUTF8Path(name); // normalize due to disk export!
	});
	// obsolete since 1.30 if not earlier. It used to be tied to random nations, then custom nations.
	// Now custom nations store their name in the same name field as regular ones, making identification bit more difficult.
	registerKeyword("custom_name", [this](const std::string& unused, std::istream& theStream) {
		randomName = V2::Localisation::convert(commonItems::singleString(theStream).getString());
		customNation = true;
	});
	registerKeyword("adjective", [this](const std::string& unused, std::istream& theStream) {
		adjective = commonItems::singleString(theStream).getString();
	});
	// This is obsolete and not applicable from at least 1.19+, probably further back
	registerKeyword("map_color", [this](const std::string& unused, std::istream& theStream) {
		auto colorColor = commonItems::Color::Factory{}.getColor(theStream);
		colorColor.RandomlyFluctuate(30);
		// Countries whose colors are included in the object here tend to be generated countries,
		// i.e. colonial nations which take on the color of their parent. To help distinguish
		// these countries from their parent's other colonies we randomly adjust the color.
		nationalColors.setMapColor(colorColor);
	});
	registerKeyword("colors", [this](const std::string& colorsString, std::istream& theStream) {
		nationalColors = NationalSymbol(theStream);
	});
	registerKeyword("capital", [this](const std::string& unused, std::istream& theStream) {
		capital = commonItems::singleInt(theStream).getInt();
	});
	registerKeyword("technology_group", [this](const std::string& unused, std::istream& theStream) {
		techGroup = commonItems::singleString(theStream).getString();
	});
	registerKeyword("liberty_desire", [this](const std::string& unused, std::istream& theStream) {
		libertyDesire = commonItems::singleDouble(theStream).getDouble();
	});
	registerKeyword("institutions", [this](const std::string& unused, std::istream& theStream) {
		for (auto institution: commonItems::intList(theStream).getInts())
			if (institution == 1)
				embracedInstitutions.push_back(true);
			else
				embracedInstitutions.push_back(false);
	});
	registerKeyword("isolationism", [this](const std::string& unused, std::istream& theStream) {
		isolationism = commonItems::singleInt(theStream).getInt();
	});
	registerKeyword("primary_culture", [this](const std::string& unused, std::istream& theStream) {
		primaryCulture = commonItems::singleString(theStream).getString();
	});
	registerKeyword("religion", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString theReligion(theStream);
		religion = theReligion.getString();
	});
	// Obsolete since 1.26.0
	registerKeyword("score", [this](const std::string& unused, std::istream& theStream) {
		score = commonItems::singleDouble(theStream).getDouble();
	});
	// Relevant since 1.20 but we only use it for 1.26+
	registerKeyword("age_score", [this, theVersion](const std::string& unused, std::istream& theStream) {
		if (theVersion >= GameVersion("1.26.0.0"))
			for (auto& agScore: commonItems::doubleList(theStream).getDoubles())
				score += agScore;
		else
			commonItems::ignoreItem(unused, theStream);
	});
	registerKeyword("stability", [this](const std::string& unused, std::istream& theStream) {
		stability = commonItems::singleDouble(theStream).getDouble();
	});
	registerKeyword("technology", [this](const std::string& unused, std::istream& theStream) {
		const EU4Technology techBlock(theStream);
		admTech = techBlock.getAdm();
		dipTech = techBlock.getDip();
		milTech = techBlock.getMil();
	});
	registerRegex("flags|hidden_flags|variables", [this](const std::string& unused, std::istream& theStream) {
		const EU4CountryFlags flagsBlock(theStream);
		for (const auto& flag: flagsBlock.getFlags())
			flags.insert(flag);
	});
	registerKeyword("modifier", [this](const std::string& unused, std::istream& theStream) {
		const EU4CountryModifier newModifier(theStream);
		if (!newModifier.getModifier().empty())
			modifiers.insert(newModifier.getModifier());
	});
	registerKeyword("government", [this, theVersion](const std::string& unused, std::istream& theStream) {
		if (theVersion < GameVersion("1.23.0.0"))
		{
			government = commonItems::singleString(theStream).getString();
		}
		else
		{
			const GovernmentSection theSection(theStream);
			government = theSection.getGovernment();
			governmentReforms = theSection.getGovernmentReforms();
		}
	});
	registerKeyword("active_relations", [this](const std::string& unused, std::istream& theStream) {
		const EU4Relations activeRelations(theStream);
		relations = activeRelations.getRelations();
	});
	registerRegex("army|navy", [this](const std::string& armyFloats, std::istream& theStream) {
		const EU4Army theArmy(theStream, armyFloats);
		armies.push_back(theArmy);
	});
	registerKeyword("active_idea_groups", [this](const std::string& unused, std::istream& theStream) {
		const EU4ActiveIdeas activeIdeas(theStream);
		nationalIdeas = activeIdeas.getActiveIdeas();
	});
	registerRegex("legitimacy|horde_unity|devotion|meritocracy|republican_tradition", [this](const std::string& legitimacyType, std::istream& theStream) {
		const commonItems::singleDouble theLegitimacy(theStream);
		if (legitimacyType == "legitimacy" || legitimacyType == "meritocracy")
			legitimacy = theLegitimacy.getDouble();
		if (legitimacyType == "horde_unity")
			hordeUnity = theLegitimacy.getDouble();
		if (legitimacyType == "devotion")
			devotion = theLegitimacy.getDouble();
		if (legitimacyType == "republican_tradition")
			republicanTradition = theLegitimacy.getDouble();
	});
	registerKeyword("average_autonomy", [this](const std::string& unused, std::istream& theStream) {
		averageAutonomy = commonItems::singleDouble(theStream).getDouble();
	});
	registerKeyword("parent", [this](const std::string& unused, std::istream& theStream) {
		commonItems::ignoreItem(unused, theStream);
		colony = true;
	});
	registerKeyword("colonial_parent", [this](const std::string& unused, std::istream& theStream) {
		commonItems::ignoreItem(unused, theStream);
		colony = true;
	});
	registerKeyword("overlord", [this](const std::string& unused, std::istream& theStream) {
		overlord = commonItems::singleString(theStream).getString();
	});
	// This is obsolete and not applicable from at least 1.19+, probably further back:
	// In current save game implementation, custom_colors stores a color triplet, but apparently it used to
	// store a custom colors block with flag and symbol - which is now in colors block.
	registerKeyword("country_colors", [this](const std::string& unused, std::istream& theStream) {
		nationalColors.setCustomColors(std::make_shared<CustomColors>(theStream));
	});
	// This is obsolete and not applicable from at least 1.19+, probably further back
	registerKeyword("revolutionary_colors", [this](const std::string& unused, std::istream& theStream) {
		nationalColors.setRevolutionaryColor(commonItems::Color::Factory{}.getColor(theStream));
	});
	registerKeyword("history", [this](const std::string& unused, std::istream& theStream) {
		const CountryHistory theCountryHistory(theStream);
		historicalLeaders = theCountryHistory.getLeaders();
		if (!theCountryHistory.getDynasty().empty())
			historicalEntry.lastDynasty = theCountryHistory.getDynasty();
		historicalPrimaryCulture = theCountryHistory.getPrimaryCulture();
		historicalReligion = theCountryHistory.getReligion();
	});
	registerKeyword("leader", [this](const std::string& unused, std::istream& theStream) {
		const LeaderID idBlock(theStream);
		activeLeaderIDs.insert(idBlock.getIDNum());
	});
	registerKeyword("active_policy", [this](const std::string& unused, std::istream& theStream) {
		const EU4ActivePolicy policyBlock(theStream);
		policies.insert(policyBlock.getPolicy());
	});
	registerKeyword("absolutism", [this](const std::string& unused, std::istream& theStream) {
		absolutism = commonItems::singleDouble(theStream).getDouble();
	});
	registerKeyword("army_tradition", [this](const std::string& unused, std::istream& theStream) {
		armyTradition = commonItems::singleDouble(theStream).getDouble();
	});
	registerKeyword("navy_tradition", [this](const std::string& unused, std::istream& theStream) {
		navyTradition = commonItems::singleDouble(theStream).getDouble();
	});
	registerKeyword("army_professionalism", [this](const std::string& unused, std::istream& theStream) {
		armyProfessionalism = commonItems::singleDouble(theStream).getDouble();
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

double EU4::Country::getLegitimacy() const
{
	if (government == "republic")
		return republicanTradition;
	if (government == "theocracy")
		return devotion;
	if (government == "monarchy")
		return legitimacy;
	if (government == "tribe")
		return hordeUnity;
	return 50;
}

void EU4::Country::determineJapaneseRelations()
{
	// pre-dharma
	if (government == "daimyo")
		possibleDaimyo = true;
	if (government == "shogunate")
		possibleShogun = true;

	// post-dharma
	if (governmentReforms.count("shogunate"))
		possibleShogun = true;
	if (governmentReforms.count("indep_daimyo"))
		possibleDaimyo = true;
	if (governmentReforms.count("daimyo"))
		possibleDaimyo = true;
}

void EU4::Country::determineInvestments(const mappers::IdeaEffectMapper& ideaEffectMapper)
{
	std::map<std::string, int> appliedIdeas;

	// Let's not do this twice. Merge ideas and reforms, they are all the same anyway.
	std::set<std::string> mergedIdeas(nationalIdeas);
	mergedIdeas.insert(governmentReforms.begin(), governmentReforms.end());

	for (const auto& idea: mergedIdeas)
	{
		// Two upper house shares for the Aristocrats under the sky
		if (ideaEffectMapper.getReactionaryFromIdea(idea))
		{
			reactionary += *ideaEffectMapper.getReactionaryFromIdea(idea);
			appliedIdeas["reactionary"]++;
		}
		if (ideaEffectMapper.getLiberalFromIdea(idea))
		{
			liberal += *ideaEffectMapper.getLiberalFromIdea(idea);
			appliedIdeas["liberal"]++;
		}

		// Three national values for bureaucrats in their halls of stone
		if (ideaEffectMapper.getLibertyFromIdea(idea))
		{
			liberty += *ideaEffectMapper.getLibertyFromIdea(idea);
			appliedIdeas["liberty"]++;
		}
		if (ideaEffectMapper.getEqualityFromIdea(idea))
		{
			equality += *ideaEffectMapper.getEqualityFromIdea(idea);
			appliedIdeas["equality"]++;
		}
		if (ideaEffectMapper.getOrderFromIdea(idea))
		{
			order += *ideaEffectMapper.getOrderFromIdea(idea);
			appliedIdeas["order"]++;
		}

		// Five technologies for the Clergymen scribbling with bone
		if (ideaEffectMapper.getArmyFromIdea(idea))
		{
			army += *ideaEffectMapper.getArmyFromIdea(idea);
			appliedIdeas["army"]++;
		}
		if (ideaEffectMapper.getCommerceFromIdea(idea))
		{
			commerce += *ideaEffectMapper.getCommerceFromIdea(idea);
			appliedIdeas["commerce"]++;
		}
		if (ideaEffectMapper.getCultureFromIdea(idea))
		{
			culture += *ideaEffectMapper.getCultureFromIdea(idea);
			appliedIdeas["culture"]++;
		}
		if (ideaEffectMapper.getIndustryFromIdea(idea))
		{
			industry += *ideaEffectMapper.getIndustryFromIdea(idea);
			appliedIdeas["industry"]++;
		}
		if (ideaEffectMapper.getNavyFromIdea(idea))
		{
			navy += *ideaEffectMapper.getNavyFromIdea(idea);
			appliedIdeas["navy"]++;
		}

		// Eight political reforms for mortal Farmers doomed to die
		if (ideaEffectMapper.getSlaveryFromIdea(idea))
		{
			slavery += *ideaEffectMapper.getSlaveryFromIdea(idea);
			appliedIdeas["slavery"]++;
		}
		if (ideaEffectMapper.getUpper_house_compositionFromIdea(idea))
		{
			upper_house_composition += *ideaEffectMapper.getUpper_house_compositionFromIdea(idea);
			appliedIdeas["upper_house_composition"]++;
		}
		if (ideaEffectMapper.getVote_franchiseFromIdea(idea))
		{
			vote_franchise += *ideaEffectMapper.getVote_franchiseFromIdea(idea);
			appliedIdeas["vote_franchise"]++;
		}
		if (ideaEffectMapper.getVoting_systemFromIdea(idea))
		{
			voting_system += *ideaEffectMapper.getVoting_systemFromIdea(idea);
			appliedIdeas["voting_system"]++;
		}
		if (ideaEffectMapper.getPublic_meetingsFromIdea(idea))
		{
			public_meetings += *ideaEffectMapper.getPublic_meetingsFromIdea(idea);
			appliedIdeas["public_meetings"]++;
		}
		if (ideaEffectMapper.getPress_rightsFromIdea(idea))
		{
			press_rights += *ideaEffectMapper.getPress_rightsFromIdea(idea);
			appliedIdeas["press_rights"]++;
		}
		if (ideaEffectMapper.getTrade_unionsFromIdea(idea))
		{
			trade_unions += *ideaEffectMapper.getTrade_unionsFromIdea(idea);
			appliedIdeas["trade_unions"]++;
		}
		if (ideaEffectMapper.getPolitical_partiesFromIdea(idea))
		{
			political_parties += *ideaEffectMapper.getPolitical_partiesFromIdea(idea);
			appliedIdeas["political_parties"]++;
		}

		// One for the Capitalist on his cushy throne
		if (ideaEffectMapper.getLiteracyFromIdea(idea))
		{
			literacy += *ideaEffectMapper.getLiteracyFromIdea(idea);
			appliedIdeas["literacy"]++;
		}
	}

	// We actually need the averages, so.

	if (appliedIdeas["army"])
		army /= appliedIdeas["army"];
	if (appliedIdeas["commerce"])
		commerce /= appliedIdeas["commerce"];
	if (appliedIdeas["culture"])
		culture /= appliedIdeas["culture"];
	if (appliedIdeas["industry"])
		industry /= appliedIdeas["industry"];
	if (appliedIdeas["navy"])
		navy /= appliedIdeas["navy"];

	if (appliedIdeas["slavery"])
		slavery /= appliedIdeas["slavery"];
	if (appliedIdeas["upper_house_composition"])
		upper_house_composition /= appliedIdeas["upper_house_composition"];
	if (appliedIdeas["vote_franchise"])
		vote_franchise /= appliedIdeas["vote_franchise"];
	if (appliedIdeas["voting_system"])
		voting_system /= appliedIdeas["voting_system"];
	if (appliedIdeas["public_meetings"])
		public_meetings /= appliedIdeas["public_meetings"];
	if (appliedIdeas["press_rights"])
		press_rights /= appliedIdeas["press_rights"];
	if (appliedIdeas["trade_unions"])
		trade_unions /= appliedIdeas["trade_unions"];
	if (appliedIdeas["political_parties"])
		political_parties /= appliedIdeas["political_parties"];

	if (appliedIdeas["liberty"])
		liberty /= appliedIdeas["liberty"];
	if (appliedIdeas["equality"])
		equality /= appliedIdeas["equality"];
	if (appliedIdeas["order"])
		order /= appliedIdeas["order"];
	if (appliedIdeas["literacy"])
		literacy /= appliedIdeas["literacy"];

	if (appliedIdeas["reactionary"])
		reactionary /= appliedIdeas["reactionary"];
	if (appliedIdeas["liberal"])
		liberal /= appliedIdeas["liberal"];
}

void EU4::Country::determineLibertyDesire()
{
	// Most of this function relates exclusively to old saves, approx. 1.26 (and potentially earlier).
	// Colonies in current saves have only two values - loyal (<50) or disloyal (>50), as this is a calculated
	// and not stored value, and colonies cannot be outraged, threatened, disloyal, etc.
	// So, we make due in V2's portion of relationship conversion, while leaving this for legacy reasons.

	if (!colony)
		return;

	const auto& relationship = relations.find(overlord);
	if (relationship != relations.end())
	{
		const auto& attitude = relationship->second.getAttitude();
		if (attitude == "attitude_rebellious")
		{
			libertyDesire = 95.0;
		}
		else if (attitude == "attitude_disloyal" || attitude == "attitude_disloyal_vassal") // _vassal for pre-1.14 games
		{
			libertyDesire = 90.0;
		}
		else if (attitude == "attitude_outraged")
		{
			libertyDesire = 85.0;
		}
		else if (attitude == "attitude_rivalry")
		{
			libertyDesire = 80.0;
		}
		else if (attitude == "attitude_hostile")
		{
			libertyDesire = 75.0;
		}
		else if (attitude == "attitude_threatened")
		{
			libertyDesire = 65.0;
		}
		else if (attitude == "attitude_neutral")
		{
			libertyDesire = 50.0;
		}
		else if (attitude == "attitude_defensive")
		{
			libertyDesire = 35.0;
		}
		else if (attitude == "attitude_domineering")
		{
			libertyDesire = 20.0;
		}
		else if (attitude == "attitude_protective")
		{
			libertyDesire = 15.0;
		}
		else if (attitude == "attitude_allied" || attitude == "attitude_friendly")
		{
			libertyDesire = 10.0;
		}
		else if (attitude == "attitude_loyal" || attitude == "attitude_overlord" || attitude == "attitude_vassal") // _vassal for pre-1.14 games
		{
			libertyDesire = 5.0;
		}
		else
		{
			LOG(LogLevel::Warning) << "Unknown attitude type " << attitude << " while setting liberty desire for " << tag;
			libertyDesire = 95.0;
		}
	}
}

void EU4::Country::readFromCommonCountry(const std::string& fileName, const std::string& fullFileName)
{
	if (name.empty())
	{
		// For this country's name we will use the stem of the file name.
		const auto extPos = fileName.find_last_of('.');
		name = fileName.substr(0, extPos);
	}

	if (!nationalColors.getMapColor())
	{
		registerKeyword("color", [this](const std::string& unused, std::istream& theStream) {
			nationalColors.setMapColor(commonItems::Color::Factory{}.getColor(theStream));
		});
		registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);

		parseFile(fullFileName);
	}
}

void EU4::Country::setLocalizationName(const std::string& language, const std::string& incName)
{
	// This is a workaround for vic2 crashes when there are 1-letter names for countries in localizations.
	// So, instead of U, there is now UU. Yay, victory.

	if (incName.size() == 1)
	{
		const auto newName = incName + incName;
		namesByLanguage[language] = newName;
	}
	else
	{
		namesByLanguage[language] = incName;
	}
}

void EU4::Country::setLocalizationAdjective(const std::string& language, const std::string& incAdjective)
{
	adjectivesByLanguage[language] = incAdjective;
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
	return modifiers.count(modifier);
}

bool EU4::Country::hasNationalIdea(const std::string& ni) const
{
	return nationalIdeas.count(ni);
}

bool EU4::Country::hasFlag(const std::string& flag) const
{
	return flags.count(flag);
}

bool EU4::Country::hasReform(const std::string& reform) const
{
	return governmentReforms.count(reform);
}

void EU4::Country::resolveRegimentTypes(const mappers::UnitTypeMapper& unitTypeMapper)
{
	for (auto& theArmy: armies)
		theArmy.resolveRegimentTypes(unitTypeMapper);
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

	LOG(LogLevel::Info) << " - " << tag << " is assimilating " << target->getTag();

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
			province->setOwnerString(tag);
			province->setControllerString(tag);
			addProvince(province);
		}

		// acquire target's armies, navies, admirals, and generals
		armies.insert(armies.end(), target->armies.begin(), target->armies.end());
		militaryLeaders.insert(militaryLeaders.end(), target->militaryLeaders.begin(), target->militaryLeaders.end());

		// re-balance prestige and techs from weighted average
		score = myWeight * score + targetWeight * target->score;
		admTech = myWeight * admTech + targetWeight * target->admTech;
		dipTech = myWeight * dipTech + targetWeight * target->dipTech;
		milTech = myWeight * milTech + targetWeight * target->milTech;
		army = myWeight * army + targetWeight * target->army;
		navy = myWeight * navy + targetWeight * target->navy;
		commerce = myWeight * commerce + targetWeight * target->commerce;
		industry = myWeight * industry + targetWeight * target->industry;
		culture = myWeight * culture + targetWeight * target->culture;
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

bool EU4::Country::cultureSurvivesInCores(const std::map<std::string, std::shared_ptr<Country>>& theCountries)
{
	// If this were a LANDLESS country, is there some other country that contains our people?
	// If so, we shouldn't delete this country as it may reappear.

	for (const auto& core: cores)
	{
		if (core->getOwnerString().empty()) // this core province belongs to noone.
			continue;
		if (core->getCulturePercent(primaryCulture) < 0.5) // this core province doesn't have our people.
			continue;

		auto owner = theCountries.find(core->getOwnerString());											  // ownerstring will be someone else, not us.
		if (owner != theCountries.end() && owner->second->getPrimaryCulture() != primaryCulture) // and they are not us, ethnically.
			return true;
	}
	return false;
}

std::string EU4::Country::getName(const std::string& language) const
{
	if (!randomName.empty()) // this applies for 1.29 and below.
		return randomName;

	// We're returning english base name as a default for all languages where we lack localization.
	if (namesByLanguage.empty())
		return name;

	if (const auto& findIter = namesByLanguage.find(language); findIter != namesByLanguage.end())
	{
		// Default to english base name for incomplete localization
		if (findIter->second.empty())
			return name;
		return findIter->second;
	}
	return name;
}

std::string EU4::Country::getAdjective(const std::string& language) const
{
	if (!randomName.empty())
		return randomName;

	// For dynamic countries there are no localizations save for the save game one,
	// so we return english for all languages.
	if (adjectivesByLanguage.empty())
		return adjective;

	const auto& engIter = adjectivesByLanguage.find("english");
	if (engIter == adjectivesByLanguage.end())
	{
		// localizations haven't loaded at all, otherwise this would exist. Bail.
		return adjective;
	}

	if (const auto& findIter = adjectivesByLanguage.find(language); findIter != adjectivesByLanguage.end())
	{
		// Default to english for incomplete localization
		if (findIter->second.empty())
			return engIter->second;
		return findIter->second;
	}
	// We're returning english adjective as a default for all languages where we lack localization.
	return engIter->second;
}

int EU4::Country::numEmbracedInstitutions() const
{
	auto total = 0;
	for (auto institution: embracedInstitutions)
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
