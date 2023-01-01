#include "FlagCrafter.h"
#include "CountryMapper/CountryMapper.h"
#include "FlagNameLoader/FlagNameLoader.h"
#include "Log.h"
#include "PoliticalManager/Country/Country.h"

void V3::FlagCrafter::loadAvailableFlags(const std::string& blankModPath, const std::string& vanillaPath)
{
	Log(LogLevel::Info) << "-> Loading Available Flags.";

	loadKnownFlags(blankModPath, vanillaPath);
	filterKnownFlags();

	Log(LogLevel::Info) << "<> Loaded " << knownFlags.size() << " available flags for " << availableFlags.size() << " entitites, and "
							  << knownVanillaFlags.size() << " vanilla flagsets.";
}

void V3::FlagCrafter::loadCustomColors(const std::string& filePath)
{
	flagColorLoader.loadFlagColors(filePath);
}

void V3::FlagCrafter::loadKnownFlags(const std::string& blankModPath, const std::string& vanillaPath)
{
	FlagNameLoader flagNameLoader;
	flagNameLoader.loadKnownFlags(blankModPath); // we're loading our COA DEFINITIONS, not flag definitions!
	knownFlags = flagNameLoader.getKnownFlags();
	FlagNameLoader vanillaFlagNameLoader;
	vanillaFlagNameLoader.loadKnownFlags(vanillaPath); // Now we're loading vanilla flag DEFINITIONS for tags, directly.
	knownVanillaFlags = vanillaFlagNameLoader.getKnownFlags();
}

void V3::FlagCrafter::filterKnownFlags()
{
	for (const auto& flag: knownFlags)
	{
		auto flagName = flag;
		FLAGTYPE flagType = Default;

		if (flagName.starts_with("legacy_") && flagName.size() > 7)
			flagName = flagName.substr(7, flagName.size());
		if (flag.ends_with("_republic") && flag.size() > 10)
		{
			flagName = flagName.substr(0, flagName.size() - 9);
			flagType = Republic;
		}
		else if (flag.ends_with("_monarchy") && flag.size() > 10)
		{
			flagName = flagName.substr(0, flagName.size() - 9);
			flagType = Monarchy;
		}
		else if (flag.ends_with("_communist") && flag.size() > 10)
		{
			flagName = flagName.substr(0, flagName.size() - 10);
			flagType = Communist;
		}
		else if (flag.ends_with("_fascist") && flag.size() > 10)
		{
			flagName = flagName.substr(0, flagName.size() - 8);
			flagType = Fascist;
		}

		if (!availableFlags.contains(flagName))
			availableFlags.emplace(flagName, std::map<FLAGTYPE, std::string>{});
		availableFlags.at(flagName).emplace(flagType, flag);
	}
}

std::optional<std::map<V3::FlagCrafter::FLAGTYPE, std::string>> V3::FlagCrafter::getFlagsForEntity(const std::string& name)
{
	if (const auto& itr = availableFlags.find(name); itr != availableFlags.end() && !spentFlags.contains(name))
	{
		spentFlags.emplace(name);
		return itr->second;
	}
	// Now get creative.
	if (const auto& itr = availableFlags.find("e_" + name); itr != availableFlags.end() && !spentFlags.contains("e_" + name))
	{
		spentFlags.emplace("e_" + name);
		return itr->second;
	}
	if (const auto& itr = availableFlags.find("k_" + name); itr != availableFlags.end() && !spentFlags.contains("k_" + name))
	{
		spentFlags.emplace("k_" + name);
		return itr->second;
	}
	if (const auto& itr = availableFlags.find("d_" + name); itr != availableFlags.end() && !spentFlags.contains("d_" + name))
	{
		spentFlags.emplace("d_" + name);
		return itr->second;
	}
	if (const auto& itr = availableFlags.find("c_" + name); itr != availableFlags.end() && !spentFlags.contains("c_" + name))
	{
		spentFlags.emplace("c_" + name);
		return itr->second;
	}

	return std::nullopt;
}

void V3::FlagCrafter::distributeAvailableFlags(const std::map<std::string, std::shared_ptr<Country>>& countries, const mappers::CountryMapper& countryMapper)
{
	Log(LogLevel::Info) << "-> Distributing Available Flags.";
	auto flagCodeCounter = 0;
	auto tagCounter = 0;
	auto nameCounter = 0;
	auto vanillaCounter = 0;

	for (const auto& [tag, country]: countries)
	{
		// are we already set?
		if (knownVanillaFlags.contains(tag))
		{
			++vanillaCounter;
			continue;
		}

		// do we have a flagcode? That'd be super helpful.
		if (const auto& flagCode = countryMapper.getFlagCode(tag); flagCode && tryAssigningFlagViaValue(country, *flagCode))
		{
			++flagCodeCounter;
			continue;
		}

		// try straight tag.
		if (tryAssigningFlagViaValue(country, tag))
		{
			++tagCounter;
			continue;
		}

		// umm. english name? yes?
		auto name = country->getName("english");
		std::ranges::transform(name.begin(), name.end(), name.begin(), ::tolower);
		if (tryAssigningFlagViaValue(country, name))
		{
			++nameCounter;
			continue;
		}
	}

	Log(LogLevel::Info) << "<> Distributed flags for " << vanillaCounter + flagCodeCounter + tagCounter + nameCounter << " out of " << countries.size()
							  << " countries.";
	Log(LogLevel::Debug) << "Specifically, " << flagCodeCounter << " via flag codes, " << tagCounter << " via tag matches, " << nameCounter
								<< " via name matches, and " << vanillaCounter << " have vanilla flags.";
}

bool V3::FlagCrafter::tryAssigningFlagViaValue(const std::shared_ptr<Country>& country, const std::string& value)
{
	// strip legacy if we have one.
	auto actualValue = value;
	if (actualValue.starts_with("legacy_") && value.size() > 7)
		actualValue = value.substr(7, value.size());

	if (const auto& match = getFlagsForEntity(actualValue); match)
	{
		country->setFlags(*match);
		return true;
	}
	return false;
}