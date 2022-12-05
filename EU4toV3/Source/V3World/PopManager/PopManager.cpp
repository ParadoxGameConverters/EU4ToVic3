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

std::optional<V3::SubStatePops> V3::PopManager::getVanillaSubStatePops(const std::string& stateName, const std::string& ownerTag) const
{
	if (!vanillaStatePops.contains(stateName))
		return std::nullopt;
	const auto& subStatePops = vanillaStatePops.at(stateName).getSubStatePops();
	for (const auto& subStatePop: subStatePops)
		if (subStatePop.getTag() == ownerTag)
			return subStatePop;
	return std::nullopt;
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
			// skip imported substates, those already have pops and carry no demographics.
			if (subState->getSubStatePops().getPopCount() > 0)
				continue;
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

std::string V3::PopManager::getDominantVanillaReligion(const std::string& stateName) const
{
	if (!vanillaStatePops.contains(stateName))
	{
		Log(LogLevel::Warning) << "PopManager knows nothing about vanilla state " << stateName;
		return "noreligion";
	}

	const auto best = vanillaStatePops.at(stateName).getDominantReligion();
	if (!best)
	{
		Log(LogLevel::Warning) << "Vanilla state " << stateName << " is dry!";
		return "noreligion";
	}

	return *best;
}

void V3::PopManager::generatePops(const ClayManager& clayManager) const
{
	Log(LogLevel::Info) << "-> Generating Pops.";

	/* EU4-imported substates have their own weights. These weights stem from chunks and prior from EU4 province development,
	 * and we need to distribute the statePopcount according to those weights. However.
	 *
	 * When looking at a mixed state, the imported and shoved substates won't have a weight, but we still need to account for
	 * them having - or not having - pops!
	 *
	 * FIRST assign *something* to shoved substates, then count existing (imported + shoved) pops, substract that from total pops,
	 * and only then distribute the remainder accoding to weights. Tad involved but no way around it.
	 */

	for (const auto& [stateName, state]: clayManager.getStates())
	{
		if (state->isSea() || state->isLake())
			continue;

		if (!vanillaStatePops.contains(stateName))
			continue;

		// TODO: ALTER this later! Use imagination! Use functions! Compare this number to combined state weights of the continent
		// ... or something.
		const auto vanillaStatePopCount = vanillaStatePops.at(stateName).getPopCount();

		/* Now there's a few things to keep in mind. The substate we're operating on can be any of these:
		 *
		 * 1. Normally generated from EU4, carrying its own demographics
		 * 2. Imported from vanilla map, carrying its own (mostly ?) vanilla pops.
		 * 3. Shoved from unassigned provinces, carrying nothing whatsoever.
		 *
		 * Right now we're concerned only with 3, and after that 1. 2 doesn't need work done.
		 * */

		// what is our unassigned count and total weight?
		auto unassignedPopCount = vanillaStatePopCount - state->getStatePopCount();

		// assigned provinces are provinces in substates that have pops inside. We need to exclude them.
		const auto assignedProvinces = std::accumulate(state->getSubStates().begin(), state->getSubStates().end(), 0, [](int sum, const auto& subState) {
			if (subState->getSubStatePops().getPopCount() > 0)
				sum += static_cast<int>(subState->getProvinces().size());
			return sum;
		});
		const auto unassignedProvinces = static_cast<int>(state->getProvinces().size()) - assignedProvinces;

		for (const auto& subState: state->getSubStates())
		{
			if (subState->getSubStatePops().getPopCount() > 0)
				continue;

			if (subState->getDemographics().empty())
			{
				const auto generatedPopCount = generatePopCountForShovedSubState(subState, unassignedPopCount, unassignedProvinces);
				// We have no demographics! Use best guess.
				auto pop = Pop(getDominantVanillaCulture(stateName), getDominantVanillaCulture(stateName), "", generatedPopCount);
				subState->addPop(pop);
				// and we're done with this one.
			}
		}
		unassignedPopCount = vanillaStatePopCount - state->getStatePopCount();

		// now iterate again and distribute that unassigned count according to weights.

		for (const auto& subState: state->getSubStates())
		{
			if (subState->getSubStatePops().getPopCount() > 0)
				continue;

			if (!subState->getWeight())
			{
				Log(LogLevel::Warning) << "Substate in " << stateName << " has NO WEIGHT! It's supposed to be imported from EU4! Not generating pops!";
				continue;
			}

			const auto generatedPopCount = generatePopCountForNormalSubState(subState, unassignedPopCount);
			subState->generatePops(generatedPopCount);
		}
	}

	const auto worldSum = std::accumulate(clayManager.getStates().begin(), clayManager.getStates().end(), 0, [](int sum, const auto& state) {
		return sum + state.second->getStatePopCount();
	});

	Log(LogLevel::Info) << "<> World now has " << worldSum << " pops.";
}

int V3::PopManager::generatePopCountForNormalSubState(const std::shared_ptr<SubState>& subState, int unassignedPopCount) const
{
	const auto& state = subState->getHomeState();
	const double subStateWeightRatio = *subState->getWeight() / state->getTotalSubStateWeight();

	// Size doesn't matter, only weight does. If our weight is 100 (from combined dev etc), and entire state had 1000 weight, we get 10% pops
	// regardles of how big the substate is.

	return static_cast<int>(round(subStateWeightRatio * static_cast<double>(unassignedPopCount)));
}

int V3::PopManager::generatePopCountForShovedSubState(const std::shared_ptr<SubState>& subState, int unassignedPopCount, int unassignedProvinces) const
{
	// TODO: Use a better method. Any method is better than this:

	const double subStateRatio = static_cast<double>(subState->getProvinces().size()) / static_cast<double>(unassignedProvinces);
	return static_cast<int>(round(subStateRatio * static_cast<double>(unassignedPopCount)));
}
