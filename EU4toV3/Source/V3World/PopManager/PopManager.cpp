#include "PopManager.h"
#include "ClayManager/ClayManager.h"
#include "ClayManager/State/Chunk.h"
#include "ClayManager/State/State.h"
#include "ClayManager/State/SubState.h"
#include "CultureMapper/CultureMapper.h"
#include "Loaders/PopLoader/PopLoader.h"
#include "Log.h"
#include "ReligionMapper/ReligionMapper.h"
#include <cmath>
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

void V3::PopManager::initializeDWPops(const commonItems::ModFilesystem& modFS)
{
	Log(LogLevel::Info) << "-> Loading Decentralized World Pops.";
	PopLoader popLoader;
	popLoader.loadPops(modFS);
	dwStatePops = popLoader.getStatePops();

	const auto total = std::accumulate(dwStatePops.begin(), dwStatePops.end(), 0, [](int sum, const std::pair<std::string, StatePops>& statePop) {
		return sum + statePop.second.getPopCount();
	});

	Log(LogLevel::Info) << "<> Decentralized World had " << total << " pops.";
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

std::optional<V3::SubStatePops> V3::PopManager::getDWSubStatePops(const std::string& stateName, const std::string& ownerTag) const
{
	if (!dwStatePops.contains(stateName))
		return std::nullopt;
	const auto& subStatePops = dwStatePops.at(stateName).getSubStatePops();
	for (const auto& subStatePop: subStatePops)
		if (subStatePop.getTag() == ownerTag)
			return subStatePop;
	return std::nullopt;
}

void V3::PopManager::convertDemographics(const ClayManager& clayManager,
	 mappers::CultureMapper& cultureMapper,
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
			if (subState->isVanillaSubState())
				continue;
			subState->convertDemographics(clayManager, cultureMapper, religionMapper, cultureLoader, religionLoader);
		}
	}
}

void V3::PopManager::applyHomeLands(const ClayManager& clayManager) const
{
	Log(LogLevel::Info) << "-> Applying Homelands.";
	auto counter = 0;
	for (const auto& [stateName, state]: clayManager.getStates())
	{
		if (state->isSea() || state->isLake())
			continue;

		if (!dwStatePops.contains(stateName))
		{
			Log(LogLevel::Warning) << "State " << stateName << " is unknown. Not processing for homelands.";
			continue;
		}

		for (const auto& subState: state->getSubStates())
		{
			// skip imported substates, those already have imported homelands into their states.
			if (subState->isVanillaSubState())
				continue;
			if (const auto& primaryCulture = subState->getPrimaryCulture(); primaryCulture)
			{
				state->addHomeland(*primaryCulture);
				++counter;
			}
		}
	}

	Log(LogLevel::Info) << "<> Applied " << counter << " homelands.";
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

std::string V3::PopManager::getDominantDWCulture(const std::string& stateName) const
{
	if (!dwStatePops.contains(stateName))
	{
		Log(LogLevel::Warning) << "PopManager knows nothing about DW state " << stateName;
		return "noculture";
	}

	const auto best = dwStatePops.at(stateName).getDominantCulture();
	if (!best)
	{
		Log(LogLevel::Warning) << "DW state " << stateName << " is dry!";
		return "noculture";
	}

	return *best;
}

std::string V3::PopManager::getDominantDWReligion(const std::string& stateName) const
{
	if (!dwStatePops.contains(stateName))
	{
		Log(LogLevel::Warning) << "PopManager knows nothing about DW state " << stateName;
		return "noreligion";
	}

	const auto best = dwStatePops.at(stateName).getDominantReligion();
	if (!best)
	{
		Log(LogLevel::Warning) << "DW state " << stateName << " is dry!";
		return "noreligion";
	}

	return *best;
}

void V3::PopManager::generatePops(const ClayManager& clayManager)
{
	const auto worldTotal = std::accumulate(vanillaStatePops.begin(), vanillaStatePops.end(), 0, [](int sum, const auto& statePops) {
		return sum + statePops.second.getPopCount();
	});
	Log(LogLevel::Info) << "-> Generating Pops. Vanilla World was: " << worldTotal << " pops.";

	filterVanillaMinorityStatePops();

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

		if (!vanillaStatePops.contains(stateName) || !dwStatePops.contains(stateName))
			continue;

		auto minorityPopCount = 0;
		if (vanillaMinorityStatePops.contains(stateName))
			minorityPopCount = vanillaMinorityStatePops.at(stateName).getPopCount();

		// TODO: ALTER this later! Use imagination! Use functions! Compare this number to combined state weights of the continent
		// TODO: ... or something.
		// do not count minorities towards vanilla population as they get added separately.
		const auto vanillaStatePopCount = vanillaStatePops.at(stateName).getPopCount() - minorityPopCount;
		// dw pops don't have minorities.
		const auto dwStatePopCount = dwStatePops.at(stateName).getPopCount();

		/* Now there's a few things to keep in mind. The substate we're operating on can be any of these:
		 *
		 * 1. Normally generated from EU4, carrying its own demographics
		 * 2. Imported from vanilla map, carrying its own (mostly ?) vanilla pops.
		 * 3. Shoved from unassigned provinces, carrying nothing whatsoever.
		 *
		 * Right now we're concerned only with 3, and after that 1. 2 doesn't need work done.
		 * */

		// assigned provinces are provinces in substates that have pops inside. We need to exclude them when building ratios.
		const auto assignedProvinceCount = std::accumulate(state->getSubStates().begin(), state->getSubStates().end(), 0, [](int sum, const auto& subState) {
			if (subState->getSubStatePops().getPopCount() > 0)
				sum += static_cast<int>(subState->getProvinces().size());
			return sum;
		});

		// what is our unassigned pop count? (original statePopCount - what's in imported substates already)
		// shoved states use dw popcount and pops
		const auto unassignedDWPopCount = dwStatePopCount - state->getStatePopCount();
		const auto unassignedProvinceCount = static_cast<int>(state->getProvinces().size()) - assignedProvinceCount;
		generatePopsForShovedSubStates(state, unassignedDWPopCount, unassignedProvinceCount);

		// now iterate again and distribute that unassigned count according to weights.
		// normal/imported states use vanilla popcount and pops (with minorities)
		const auto unassignedVanillaPopCount = vanillaStatePopCount - state->getStatePopCount();
		generatePopsForNormalSubStates(state, unassignedVanillaPopCount);
	}

	const auto worldSum = std::accumulate(clayManager.getStates().begin(), clayManager.getStates().end(), 0, [](int sum, const auto& state) {
		return sum + state.second->getStatePopCount();
	});

	const auto worldDelta = worldSum - worldTotal;
	Log(LogLevel::Info) << "<> World now has " << worldSum << " pops (delta: " << worldDelta << " pops).";
	if (std::abs(static_cast<double>(worldDelta) / static_cast<double>(worldTotal)) > 0.001)
		Log(LogLevel::Warning) << "World delta is over one permille (" << std::abs(static_cast<double>(worldDelta) / static_cast<double>(worldTotal))
									  << "). This is troubling!";
	else
		Log(LogLevel::Info) << ">> World delta at " << std::abs(static_cast<double>(worldDelta) / static_cast<double>(worldTotal)) << ". Acceptable.";
}

void V3::PopManager::filterVanillaMinorityStatePops()
{
	Log(LogLevel::Info) << "-> Filtering Vanilla Minority Pops.";
	auto counter = 0;

	for (auto& [stateName, statePops]: vanillaStatePops)
	{
		vanillaMinorityStatePops.emplace(stateName, StatePops());
		for (auto& subStatePops: statePops.getSubStatePops())
		{
			SubStatePops newSubStatePops;
			newSubStatePops.setTag(subStatePops.getTag());
			for (auto& pop: subStatePops.getPops())
			{
				auto newPop = pop;
				if (minorityPopMapper.blankMajorityFromMinority(newPop))
				{
					newSubStatePops.addPop(newPop);
					++counter;
				}
			}
			vanillaMinorityStatePops.at(stateName).addSubStatePops(newSubStatePops);
		}
	}
	Log(LogLevel::Info) << "<> Fished out " << counter << " minority pops.";
}

void V3::PopManager::generatePopsForNormalSubStates(const std::shared_ptr<State>& state, int unassignedPopCount) const
{
	const auto& stateName = state->getName();
	for (const auto& subState: state->getSubStates())
	{
		if (subState->getSubStatePops().getPopCount() > 0)
			continue;

		if (!subState->getWeight())
		{
			if (subState->getOwnerTag())
			{
				Log(LogLevel::Warning) << "Substate " << *subState->getOwnerTag() << " in " << state->getName()
											  << " has NO WEIGHT! It's supposed to be imported from EU4! Not generating pops!";
			}
			else
				Log(LogLevel::Warning) << "Substate (shoved) in " << state->getName()
											  << " has NO WEIGHT! It's supposed to be imported from EU4! Not generating pops!";
			continue;
		}

		const auto generatedPopCount = generatePopCountForNormalSubState(subState, unassignedPopCount);
		subState->generatePops(generatedPopCount);
		subState->setStageForMinorities(true);
	}

	if (!vanillaMinorityStatePops.contains(state->getName()) || vanillaMinorityStatePops.at(stateName).getPopCount() == 0)
	{
		for (const auto& subState: state->getSubStates())
			subState->setStageForMinorities(false);
		return;
	}

	for (const auto& subState: state->getSubStates())
	{
		if (!subState->isStagedForMinorities())
			continue;
		const auto generatedPopCount = generatePopCountForNormalSubState(subState, unassignedPopCount);
		// if we assigned 10k out of 80k unassigned pops to this substate, then multiply vanilla minority popcount by 1/8, and get that many pops.
		const double minorityPopSizeTotal = std::round(static_cast<double>(vanillaMinorityStatePops.at(stateName).getPopCount()) *
																	  static_cast<double>(generatedPopCount) / static_cast<double>(unassignedPopCount));
		auto minorityPops = generateMinorityPops(stateName,
			 minorityPopSizeTotal,
			 subState->getSubStatePops().getDominantCulture(),
			 subState->getSubStatePops().getDominantReligion());
		subState->addPops(minorityPops);
		subState->setStageForMinorities(false);
	}
}

void V3::PopManager::generatePopsForShovedSubStates(const std::shared_ptr<State>& state, int unassignedPopCount, int unassignedProvinceCount) const
{
	const auto& stateName = state->getName();
	for (const auto& subState: state->getSubStates())
	{
		if (subState->getSubStatePops().getPopCount() > 0)
			continue;

		if (subState->getDemographics().empty())
		{
			const auto generatedPopCount = generatePopCountForShovedSubState(subState, unassignedPopCount, unassignedProvinceCount);
			// We have no demographics! Use best guess. Also default religion to culture default.
			auto pop = Pop(getDominantDWCulture(stateName), "", "", generatedPopCount);
			subState->addPop(pop);
			// and we're done with this one.
		}
	}
}

std::vector<V3::Pop> V3::PopManager::generateMinorityPops(const std::string& stateName,
	 double neededPopSizeTotal,
	 const std::optional<std::string>& dominantCulture,
	 const std::optional<std::string>& dominantReligion) const
{
	std::vector<Pop> toReturn;
	if (!vanillaMinorityStatePops.contains(stateName) || vanillaMinorityStatePops.at(stateName).getPopCount() == 0)
		return toReturn;

	const auto vanillaPopSizeTotal = static_cast<double>(vanillaMinorityStatePops.at(stateName).getPopCount());
	for (const auto& subStatePops: vanillaMinorityStatePops.at(stateName).getSubStatePops())
	{
		for (const auto& pop: subStatePops.getPops())
		{
			auto newPop = pop;
			const double ratio = neededPopSizeTotal / vanillaPopSizeTotal;
			const double newSize = std::round(ratio * pop.getSize());
			newPop.setSize(static_cast<int>(newSize));
			if (newPop.getCulture().empty())
			{
				if (dominantCulture)
					newPop.setCulture(*dominantCulture);
				else
					continue;
			}
			if (newPop.getReligion().empty())
			{
				if (dominantReligion)
					newPop.setReligion(*dominantReligion);
				else
					continue;
			}
			toReturn.emplace_back(newPop);
		}
	}
	return toReturn;
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

void V3::PopManager::loadMinorityPopRules(const std::string& filePath)
{
	minorityPopMapper.loadMappingRules(filePath);
}

void V3::PopManager::injectReligionsIntoVanillaPops(const std::map<std::string, mappers::CultureDef>& cultureDefs)
{
	Log(LogLevel::Info) << "-> Injecting Religion into Vanilla Pops.";
	vanillaStatePops = injectReligionsIntoPops(cultureDefs, vanillaStatePops);
	Log(LogLevel::Info) << "<> Updated vanilla pops.";
}

void V3::PopManager::injectReligionsIntoDWPops(const std::map<std::string, mappers::CultureDef>& cultureDefs)
{
	Log(LogLevel::Info) << "-> Injecting Religion into Decentralized World Pops.";
	dwStatePops = injectReligionsIntoPops(cultureDefs, dwStatePops);
	Log(LogLevel::Info) << "<> Updated decentralized world pops.";
}

std::map<std::string, V3::StatePops> V3::PopManager::injectReligionsIntoPops(const std::map<std::string, mappers::CultureDef>& cultureDefs,
	 const std::map<std::string, StatePops>& pops)
{
	std::map<std::string, StatePops> newVanillaPops;
	for (const auto& [stateName, statePops]: pops)
	{
		StatePops newStatePops;
		newStatePops.setStateName(stateName);
		for (const auto& subStatePops: statePops.getSubStatePops())
		{
			SubStatePops newSubStatePops;
			newSubStatePops.setTag(subStatePops.getTag());
			for (const auto& pop: subStatePops.getPops())
			{
				auto newPop = pop;
				if (pop.getReligion().empty() && cultureDefs.contains(pop.getCulture()))
					newPop.setReligion(cultureDefs.at(pop.getCulture()).religion);

				newSubStatePops.addPop(newPop);
			}
			newStatePops.addSubStatePops(newSubStatePops);
		}
		newVanillaPops.emplace(stateName, newStatePops);
	}
	return newVanillaPops;
}
