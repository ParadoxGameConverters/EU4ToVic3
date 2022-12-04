#include "PopManager.h"
#include "ClayManager/ClayManager.h"
#include "ClayManager/State/Chunk.h"
#include "ClayManager/State/State.h"
#include "ClayManager/State/SubState.h"
#include "CultureMapper/CultureMapper.h"
#include "Loaders/PopLoader/PopLoader.h"
#include "Log.h"
#include "ReligionMapper/ReligionMapper.h"
#include <numeric>
#include <ranges>

void V3::PopManager::initializeVanillaPops(const commonItems::ModFilesystem& modFS)
{
	Log(LogLevel::Info) << "-> Loading Vanilla Pops.";
	PopLoader popLoader;
	popLoader.loadPops(modFS);
	vanillaStatePops = popLoader.getStatePops();

	const auto total = std::accumulate(vanillaStatePops.begin(), vanillaStatePops.end(), 0, [](int sum, const std::pair<std::string, StatePops>& statePop) {
		return sum + statePop.second.getPopCount();
	});

	Log(LogLevel::Info) << "<> Vanilla had " << total << " pops.";
}

void V3::PopManager::assignVanillaPopsToStates(const ClayManager& clayManager)
{
	Log(LogLevel::Info) << "-> Assigning vanilla pops to states.";

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

void V3::PopManager::convertDemographics(const ClayManager& clayManager,
	 const mappers::CultureMapper& cultureMapper,
	 const mappers::ReligionMapper& religionMapper,
	 const EU4::CultureLoader& cultureLoader,
	 const EU4::ReligionLoader& religionLoader) const
{
	Log(LogLevel::Info) << "-> Converting EU4 demographics.";

	// At this point, all substates we have are owned, and formed from sane chunks. They have incoming popratios,
	// and using regional data we can convert them into Vic3 demographics.

	for (const auto& [stateName, state]: clayManager.getStates())
	{
		if (state->isSea() || state->isLake())
			continue;

		if (!vanillaStatePops.contains(stateName))
		{
			Log(LogLevel::Warning) << "State " << stateName << " is unknown. Not creating pops.";
			continue;
		}

		for (const auto& subState: state->getSubStates())
		{
			subState->convertDemographics(clayManager, cultureMapper, religionMapper, cultureLoader, religionLoader);
		}
	}
}

std::string V3::PopManager::getDominantVanillaCulture(const std::string& stateName) const
{
	if (!vanillaStatePops.contains(stateName))
	{
		Log(LogLevel::Warning) << "PopManager knows nothing about vanilla state " << stateName;
		return "noculture";
	}

	const auto best = vanillaStatePops.at(stateName).getDominantCulture();
	if (!best)
	{
		Log(LogLevel::Warning) << "Vanilla state " << stateName << " is dry!";
		return "noculture";
	}

	return *best;
}
