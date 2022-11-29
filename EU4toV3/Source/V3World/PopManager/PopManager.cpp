#include "PopManager.h"
#include "ClayManager/ClayManager.h"
#include "CultureMapper/CultureMapper.h"
#include "Log.h"
#include "PopLoader/PopLoader.h"
#include "ReligionMapper/ReligionMapper.h"
#include <numeric>
#include <ranges>

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

void V3::PopManager::convertDemographics(const ClayManager& clayManager,
	 const mappers::CultureMapper& cultureMapper,
	 const mappers::ReligionMapper& religionMapper,
	 const EU4::CultureLoader& cultureLoader,
	 const EU4::ReligionLoader& religionLoader) const
{
	// All the substates have demographics, and using regional data we can convert them into Vic3 counterparts.

	for (const auto& [stateName, state]: clayManager.getStates())
	{
		if (!vanillaStatePops.contains(stateName))
		{
			Log(LogLevel::Warning) << "State " << stateName << " is unknown. Not creating pops.";
			continue;
		}

		for (const auto& substate: state->getSubStates())
		{
			std::vector<Demographic> newDemographics;
			for (const auto& demo: substate->getDemographics())
			{
				auto newDemo = demo;
				// Owner - no owner is fine as it covers decentralized clay.
				auto ownerTag = substate->getOwnerTag();
				if (!ownerTag)
					ownerTag = "";

				// Religion
				auto religionMatch = religionMapper.getV3Religion(demo.religion);
				if (!religionMatch)
					newDemo.religion = "noreligion";
				else
					newDemo.religion = *religionMatch;

				// Culture
				auto cultureMatch = cultureMapper.cultureMatch(clayManager, cultureLoader, religionLoader, demo.culture, demo.religion, stateName, *ownerTag);
				if (!cultureMatch)
					newDemo.culture = "noculture";
				else
					newDemo.culture = *cultureMatch;

				newDemographics.push_back(newDemo);
			}
			substate->setDemographics(newDemographics); // these are old demos updated with actual vic3 culture/religion.
		}
	}
}
