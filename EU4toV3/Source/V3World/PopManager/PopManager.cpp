#include "PopManager.h"
#include "Log.h"
#include "PopLoader/PopLoader.h"
#include <numeric>
#include <ranges>

#include "ClayManager/ClayManager.h"

void V3::PopManager::initializeVanillaPops(const std::string& v3Path)
{
	Log(LogLevel::Info) << "-> Loading Vanilla Pops.";
	PopLoader popLoader;
	popLoader.loadPops(v3Path);
	vanillaStatePops = popLoader.getStatePops();

	const auto total = std::accumulate(vanillaStatePops.begin(), vanillaStatePops.end(), 0, [](int sum, const std::pair<std::string, StatePops>& statePop) {
		return sum + statePop.second.getPopCount();
	});

	Log(LogLevel::Info) << "<> Vanilla had " << total << " pops.";
}

void V3::PopManager::assignVanillaPopsToStates(const ClayManager& clayManager)
{
	for (const auto& [stateName, statePops]: vanillaStatePops)
	{
		if (!clayManager.getStates().contains(stateName))
		{
			Log(LogLevel::Warning) << "Vanilla pops for unknown state " << stateName << " cannot be assigned!";
			continue;
		}
		clayManager.getStates().at(stateName)->setVanillaPops(statePops);
	}
}

void V3::PopManager::importDemographics(const ClayManager& clayManager)
{
	for (const auto& chunk: clayManager.getChunks())
		chunk->importDemographics();
}
