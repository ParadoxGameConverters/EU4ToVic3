#include "FlagCrafter.h"
#include "FlagNameLoader/FlagNameLoader.h"
#include "Log.h"

void V3::FlagCrafter::loadAvailableFlags(const std::string& folderPath)
{
	Log(LogLevel::Info) << "-> Loading Available Flags.";

	loadKnownFlags(folderPath);
	filterKnownFlags();

	Log(LogLevel::Info) << "<> Loaded " << availableFlags.size() << " available flags.";
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
		Log(LogLevel::Debug) << "flag |" << flagName << "| typ " << static_cast<int>(flagType);
		if (!availableFlags.contains(flagName))
			availableFlags.emplace(flagName, std::map<FLAGTYPE, std::string>{});
		availableFlags.at(flagName).emplace(flagType, flag);
	}
}

std::optional<std::map<V3::FlagCrafter::FLAGTYPE, std::string>> V3::FlagCrafter::getFlagsForEntity(const std::string& name) const
{
	if (const auto& itr = availableFlags.find(name); itr != availableFlags.end())
		return itr->second;
	return std::nullopt;
}
