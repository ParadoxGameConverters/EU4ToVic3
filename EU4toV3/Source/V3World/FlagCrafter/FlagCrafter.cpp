#include "FlagCrafter.h"
#include "FlagNameLoader/FlagNameLoader.h"
#include "Log.h"

void V3::FlagCrafter::loadAvailableFlags(const std::string& folderPath)
{
	Log(LogLevel::Info) << "-> Loading Available Flags.";

	loadKnownFlags(folderPath);
	filterKnownFlags();

	Log(LogLevel::Info) << "<> Loaded " << knownFlags.size() << " available flags for " << availableFlags.size() << " entitites.";
}

void V3::FlagCrafter::loadKnownFlags(const std::string& folderPath)
{
	FlagNameLoader flagNameLoader;
	flagNameLoader.loadKnownFlags(folderPath);
	knownFlags = flagNameLoader.getKnownFlags();
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
	if (const auto& itr = availableFlags.find(name); itr != availableFlags.end())
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
