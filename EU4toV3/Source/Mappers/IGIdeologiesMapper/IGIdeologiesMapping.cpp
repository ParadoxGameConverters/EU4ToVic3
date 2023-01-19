#include "IGIdeologiesMapping.h"
#include "ClayManager/ClayManager.h"
#include "CommonRegexes.h"
#include "CountryManager/EU4Country.h"
#include "CultureMapper/CultureMapper.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "PoliticalManager/Country/Country.h"
#include "ReligionMapper/ReligionMapper.h"

mappers::IGIdeologiesMapping::IGIdeologiesMapping(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::IGIdeologiesMapping::registerKeys()
{
	registerKeyword("culturetrait", [this](std::istream& theStream) {
		cultureTraits.emplace(commonItems::getString(theStream));
	});
	registerKeyword("religiontrait", [this](std::istream& theStream) {
		religionTraits.emplace(commonItems::getString(theStream));
	});
	registerKeyword("eu4religion", [this](std::istream& theStream) {
		eu4Religions.emplace(commonItems::getString(theStream));
	});
	registerKeyword("eu4gov", [this](std::istream& theStream) {
		eu4Govs.emplace(commonItems::getString(theStream));
	});
	registerKeyword("eu4idea", [this](std::istream& theStream) {
		eu4Ideas.emplace(commonItems::getString(theStream));
	});
	registerKeyword("eu4reform", [this](std::istream& theStream) {
		eu4Reforms.emplace(commonItems::getString(theStream));
	});
	registerKeyword("capital", [this](std::istream& theStream) {
		capitals.emplace(commonItems::getString(theStream));
	});
	registerKeyword("law", [this](std::istream& theStream) {
		laws.emplace(commonItems::getString(theStream));
	});
	registerKeyword("colony", [this](std::istream& theStream) {
		colony = (commonItems::getString(theStream) == "yes");
	});
	registerKeyword("eoc", [this](std::istream& theStream) {
		emperorOfChina = (commonItems::getString(theStream) == "yes");
	});
	registerKeyword("hremperor", [this](std::istream& theStream) {
		HREmperor = (commonItems::getString(theStream) == "yes");
	});
	registerRegex(R"(ig_\w+)", [this](const std::string& IGName, std::istream& theStream) {
		if (!IGIdeologyMods.contains(IGName))
			IGIdeologyMods.emplace(IGName, IGIdeologyMod());

		const auto mod = commonItems::getString(theStream);
		if (mod.starts_with("add:"))
			IGIdeologyMods.at(IGName).addedIdeologies.emplace(mod.substr(4, mod.size()));
		else if (mod.starts_with("remove:"))
			IGIdeologyMods.at(IGName).removedIdeologies.emplace(mod.substr(7, mod.size()));
		else
			Log(LogLevel::Warning) << "Unrecognized IG Ideology mod: " << mod << " for " << IGName << "! Skipping!";
	});

	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::map<std::string, mappers::IGIdeologyMod> mappers::IGIdeologiesMapping::alterIdeologyMods(const std::map<std::string, IGIdeologyMod>& incMods,
	 const V3::Country& country,
	 const CultureMapper& cultureMapper,
	 const ReligionMapper& religionMapper,
	 const V3::ClayManager& clayManager) const
{
	if (!matchCountry(country, cultureMapper, religionMapper, clayManager))
		return incMods;

	auto newMods = incMods;
	for (const auto& [ig, theMod]: IGIdeologyMods)
	{
		if (!newMods.contains(ig))
			newMods.emplace(ig, IGIdeologyMod());
		for (const auto& ideology: theMod.addedIdeologies)
			if (newMods.at(ig).removedIdeologies.contains(ideology))
				newMods.at(ig).removedIdeologies.erase(ideology);
			else
				newMods.at(ig).addedIdeologies.emplace(ideology);

		for (const auto& ideology: theMod.removedIdeologies)
			if (newMods.at(ig).addedIdeologies.contains(ideology))
				newMods.at(ig).addedIdeologies.erase(ideology);
			else
				newMods.at(ig).removedIdeologies.emplace(ideology);
	}
	return newMods;
}

bool mappers::IGIdeologiesMapping::matchCountry(const V3::Country& country,
	 const CultureMapper& cultureMapper,
	 const ReligionMapper& religionMapper,
	 const V3::ClayManager& clayManager) const
{
	if (!country.getSourceCountry())
		return false;
	const auto& srcCountry = country.getSourceCountry();

	if (!cultureTraits.empty())
	{
		bool match = false;
		for (const auto& trait: cultureTraits)
		{
			for (const auto& culture: country.getProcessedData().cultures)
				if (cultureMapper.getV3CultureDefinitions().contains(culture) && cultureMapper.getV3CultureDefinitions().at(culture).traits.contains(trait))
					match = true;
		}
		if (!match)
			return false;
	}

	if (!religionTraits.empty())
	{
		bool match = false;
		const auto& religion = country.getProcessedData().religion;
		for (const auto& trait: religionTraits)
			if (religionMapper.getV3ReligionDefinitions().contains(religion) && religionMapper.getV3ReligionDefinitions().at(religion).traits.contains(trait))
				match = true;
		if (!match)
			return false;
	}

	if (!eu4Religions.empty())
	{
		if (!eu4Religions.contains(srcCountry->getReligion()))
			return false;
	}

	if (!eu4Govs.empty())
	{
		if (!eu4Govs.contains(srcCountry->getGovernment()))
			return false;
	}

	if (!eu4Ideas.empty())
	{
		bool match = false;
		for (const auto& idea: eu4Ideas)
			if (srcCountry->getNationalIdeas().contains(idea))
				match = true;
		if (!match)
			return false;
	}

	if (!eu4Reforms.empty())
	{
		bool match = false;
		for (const auto& reform: eu4Reforms)
			if (srcCountry->getReforms().contains(reform))
				match = true;
		if (!match)
			return false;
	}

	if (!capitals.empty())
	{
		bool match = false;
		for (const auto& region: capitals)
			if (clayManager.stateIsInRegion(country.getProcessedData().capitalStateName, region))
				match = true;
		if (!match)
			return false;
	}

	if (!laws.empty())
	{
		bool match = false;
		for (const auto& law: laws)
			if (country.getProcessedData().laws.contains(law))
				match = true;
		if (!match)
			return false;
	}

	if (colony)
	{
		if (srcCountry->isColony() != *colony)
			return false;
	}

	if (emperorOfChina)
	{
		if (srcCountry->isCelestialEmperor() != *emperorOfChina)
			return false;
	}

	if (HREmperor)
	{
		if (srcCountry->isHREmperor() != *HREmperor)
			return false;
	}

	return true;
}
