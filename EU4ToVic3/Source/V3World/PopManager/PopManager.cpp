#include "PopManager.h"
#include "ClayManager/ClayManager.h"
#include "ClayManager/State/Chunk.h"
#include "ClayManager/State/State.h"
#include "ClayManager/State/SubState.h"
#include "CultureMapper/CultureMapper.h"
#include "Loaders/PopLoader/PopLoader.h"
#include "Loaders/SuperRegionLoader/V3Region.h"
#include "Loaders/SuperRegionLoader/V3SuperRegion.h"
#include "Log.h"
#include "PoliticalManager/Country/Country.h"
#include "PoliticalManager/PoliticalManager.h"
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
	for (const auto& state: clayManager.getStates() | std::views::values)
	{
		if (state->isSea() || state->isLake())
			continue;

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

void V3::PopManager::generatePops(const ClayManager& clayManager, const Configuration::POPSHAPES popShapes, double shapingFactor)
{
	const auto worldTotal = std::accumulate(vanillaStatePops.begin(), vanillaStatePops.end(), 0, [](int sum, const auto& statePops) {
		return sum + statePops.second.getPopCount();
	});
	Log(LogLevel::Info) << "-> Generating Pops. Vanilla World was: " << worldTotal << " pops.";

	filterVanillaMinorityStatePops();

	/* EU4-imported substates have their own weights. These weights stem from chunks and prior from EU4 province development,
	 * and we need to distribute the statePopCount according to those weights. However.
	 *
	 * When looking at a mixed state, the imported and shoved substates won't have a weight, but we still need to account for
	 * them having - or not having - pops!
	 *
	 * FIRST assign *something* to shoved substates, then count existing (imported + shoved) pops, subtract that from total pops,
	 * and only then distribute the remainder according to weights. Tad involved but no way around it.
	 */

	const auto vanillaSuperRegionPopCount = getVanillaSuperRegionalPopCounts(clayManager);		// superregion -> total popcount
	const auto vanillaSuperRegionWeights = getVanillaSuperRegionalWeights(clayManager);			// superregion -> total weight
	const auto superRegionProjectedCounts = getSuperRegionPopShapingProjections(clayManager); // superregion -> pre-normalized popshaping projection

	for (const auto& [stateName, state]: clayManager.getStates())
	{
		if (state->isSea() || state->isLake())
			continue;

		if (!vanillaStatePops.contains(stateName) || !dwStatePops.contains(stateName))
			continue;

		const auto& superRegion = clayManager.getParentSuperRegion(stateName);
		if (!superRegion)
			continue;
		const auto& superRegionName = superRegion->getName();

		auto minorityPopCount = 0;
		if (vanillaMinorityStatePops.contains(stateName))
			minorityPopCount = vanillaMinorityStatePops.at(stateName).getPopCount();
		auto vanillaStatePopCount = 0;
		double stateFactor = 1.0;

		if (popShapes == Configuration::POPSHAPES::Vanilla)
		{
			vanillaStatePopCount = vanillaStatePops.at(stateName).getPopCount();
		}
		else if (popShapes == Configuration::POPSHAPES::Extreme)
		{
			// Unlike Vic2, we're reshuffling the pops within specific superregions. There are 6 superregions in Vic3 and each of the major ones contains approx
			// same amount of pops. We're just reshuffling them internally.
			const auto totalStateWeight = state->getTotalSubStateWeight();
			if (vanillaSuperRegionWeights.contains(superRegion->getName()) && vanillaSuperRegionPopCount.contains(superRegion->getName()))
			{
				const auto totalSuperRegionWeight = vanillaSuperRegionWeights.at(superRegionName);
				const auto popModifier = totalStateWeight / totalSuperRegionWeight;

				// Keep in mind - these popcounts are tied to states, not substates. A super-high-weight substate will take most of the pops however,
				// so we're ok.
				vanillaStatePopCount = static_cast<int>(std::round(vanillaSuperRegionPopCount.at(superRegionName) * popModifier));
				// Normalize by shaping factor
				vanillaStatePopCount = static_cast<int>(std::round(vanillaStatePopCount * shapingFactor)) +
											  static_cast<int>(std::round(vanillaStatePops.at(stateName).getPopCount() * (1.0 - shapingFactor)));

				stateFactor =
					 (vanillaStatePopCount - vanillaStatePops.at(stateName).getPopCount()) / static_cast<double>(vanillaStatePops.at(stateName).getPopCount());
			}
			else
			{
				Log(LogLevel::Warning) << "State " << stateName << " doesn't have a superregion! Defaulting popcount to vanilla!";
				vanillaStatePopCount = vanillaStatePops.at(stateName).getPopCount();
			}
		}
		else if (popShapes == Configuration::POPSHAPES::PopShaping)
		{
			const auto superRegionalNormalizationFactor = vanillaSuperRegionPopCount.at(superRegionName) / superRegionProjectedCounts.at(superRegionName);
			const auto stateInvestmentFactor = 1.0 + state->getInvestmentFactor();
			const auto projectedPopCount = static_cast<double>(vanillaStatePops.at(stateName).getPopCount()) * stateInvestmentFactor;
			vanillaStatePopCount = static_cast<int>(std::round(projectedPopCount * superRegionalNormalizationFactor));
			// Normalize by shaping factor
			vanillaStatePopCount = static_cast<int>(std::round(vanillaStatePopCount * shapingFactor)) +
										  static_cast<int>(std::round(vanillaStatePops.at(stateName).getPopCount() * (1.0 - shapingFactor)));
			stateFactor =
				 (vanillaStatePopCount - vanillaStatePops.at(stateName).getPopCount()) / static_cast<double>(vanillaStatePops.at(stateName).getPopCount());
		}

		// do not count minorities towards vanilla population as they get added separately.
		vanillaStatePopCount -= static_cast<int>(minorityPopCount * stateFactor);
		// we're assuming minorities are never slaves.
		const auto vanillaStateSlavePopCount = static_cast<int>(vanillaStatePops.at(stateName).getSlavePopCount() * stateFactor);
		// dw pops don't have minorities.
		const auto dwStatePopCount = dwStatePops.at(stateName).getPopCount();
		const auto startingPopCount = static_cast<int>(state->getStatePopCount() * stateFactor);

		/* Now there's a few things to keep in mind. The substate we're operating on can be any of these:
		 *
		 * 1. Normally generated from EU4, carrying its own demographics
		 * 2. Imported from vanilla map, carrying its own (mostly ?) vanilla pops.
		 * 3. Shoved from unassigned provinces, carrying nothing whatsoever.
		 *
		 * Right now we're concerned only with 3, and after that 1. 2 doesn't need work done.
		 * */

		// assigned provinces are provinces in substates that have pops inside. We need to exclude them when building ratios.
		auto assignedProvinceCount = std::accumulate(state->getSubStates().begin(), state->getSubStates().end(), 0, [](int sum, const auto& subState) {
			if (subState->getSubStatePops().getPopCount() > 0)
				sum += static_cast<int>(subState->getProvinces().size());
			return sum;
		});

		// what is our unassigned pop count? (original statePopCount - what's in imported substates already)
		// shoved states use dw popcount and pops - we *pretend* we'll be using DW popcount for entire state
		auto unassignedDWPopCount = dwStatePopCount - startingPopCount;
		if (unassignedDWPopCount < 0)
			unassignedDWPopCount = 0;
		const auto unassignedProvinceCount = static_cast<int>(state->getProvinces().size()) - assignedProvinceCount;
		if (unassignedDWPopCount > 0)
			generatePopsForShovedSubStates(state, unassignedDWPopCount, unassignedProvinceCount);

		// now iterate again and distribute that unassigned count according to weights.
		// normal/imported states use vanilla popcount and pops (with minorities) - we *pretend* we've been using vanilla popcount from the start.
		// cap the popcap at whatever clay isn't already assigned.
		assignedProvinceCount = std::accumulate(state->getSubStates().begin(), state->getSubStates().end(), 0, [](int sum, const auto& subState) {
			if (subState->getSubStatePops().getPopCount() > 0)
				sum += static_cast<int>(subState->getProvinces().size());
			return sum;
		});

		const double unassignedClayRatio =
			 static_cast<double>(state->getProvinces().size() - assignedProvinceCount) / static_cast<double>(state->getProvinces().size());
		auto unassignedVanillaPopCount = static_cast<int>(std::round(vanillaStatePopCount * unassignedClayRatio));
		if (unassignedVanillaPopCount < 0)
			unassignedVanillaPopCount = 0;
		auto unassignedVanillaSlavePopCount = static_cast<int>(std::round(vanillaStateSlavePopCount * unassignedClayRatio));
		if (unassignedVanillaSlavePopCount < 0)
			unassignedVanillaSlavePopCount = 0;
		if (unassignedVanillaPopCount > 0)
			generatePopsForNormalSubStates(state, unassignedVanillaPopCount, unassignedVanillaSlavePopCount);
	}

	const auto worldSum = std::accumulate(clayManager.getStates().begin(), clayManager.getStates().end(), 0, [](int sum, const auto& state) {
		return sum + state.second->getStatePopCount();
	});

	const auto worldDelta = worldSum - worldTotal;
	Log(LogLevel::Info) << "<> World now has " << worldSum << " pops (delta: " << worldDelta << " pops).";
	if (std::abs(static_cast<double>(worldDelta) / static_cast<double>(worldTotal)) > 0.001)
		Log(LogLevel::Info) << ">> World delta is over one permille (" << std::abs(static_cast<double>(worldDelta) / static_cast<double>(worldTotal))
								  << "). This is troubling, but not unexpected.";
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

void V3::PopManager::generatePopsForNormalSubStates(const std::shared_ptr<State>& state, const int unassignedPopCount, const int unassignedSlavePopCount) const
{
	const auto& stateName = state->getName();

	for (const auto& subState: state->getSubStates())
	{
		if (subState->getSubStatePops().getPopCount() > 0)
			continue;

		if (!subState->getWeight())
		{
			std::string warnString = "Substate ";
			if (subState->getOwnerTag())
			{
				warnString += *subState->getOwnerTag();
			}
			else
			{
				warnString += "(shoved)";
			}
			warnString += " in " + stateName + " (provinces";
			for (const auto& pid: subState->getProvinceIDs())
			{
				warnString += " " + pid;
			}
			warnString += ") has NO WEIGHT! It's supposed to be imported from EU4! Not generating pops!";
			Log(LogLevel::Warning) << warnString;
			continue;
		}

		const auto generatedPopCount = generatePopCountForNormalSubState(subState, unassignedPopCount);
		const auto generatedSlavePopCount = generatePopCountForNormalSubState(subState, unassignedSlavePopCount);
		const auto vanillaPopCount = generatePopCountForNormalSubState(subState, vanillaStatePops.at(stateName).getPopCount());
		subState->generatePops(generatedPopCount, generatedSlavePopCount);
		subState->setStageForMinorities(true);
		subState->setVanillaPopCount(vanillaPopCount);
	}

	if (!vanillaMinorityStatePops.contains(stateName) || vanillaMinorityStatePops.at(stateName).getPopCount() == 0)
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

void V3::PopManager::loadSlaveCultureRules(const std::string& filePath)
{
	slaveCultureMapper.loadMappingRules(filePath);
}

void V3::PopManager::liberateSlaves(const PoliticalManager& politicalManager) const
{
	Log(LogLevel::Info) << "-> Liberating slaves where it is banned.";
	auto counter = 0;

	for (const auto& country: politicalManager.getCountries() | std::views::values)
	{
		if (!country->hasLaw("law_slavery_banned"))
			continue;
		for (const auto& subState: country->getSubStates())
		{
			if (subState->getSubStatePops().getSlavePopCount() == 0)
				continue;
			auto newSubStatePops = subState->getSubStatePops();
			std::vector<Pop> newPops;
			for (const auto& pop: subState->getSubStatePops().getPops())
			{
				if (pop.getType() == "slaves")
				{
					Pop newPop = pop;
					newPop.setType("");
					newPops.emplace_back(newPop);
					++counter;
				}
				else
				{
					newPops.emplace_back(pop);
				}
			}
			newSubStatePops.setPops(newPops);
			subState->setSubStatePops(newSubStatePops);
		}
	}
	Log(LogLevel::Info) << "<> Liberated " << counter << " Slave Pops.";
}

void V3::PopManager::alterSlaveCultures(const PoliticalManager& politicalManager,
	 const ClayManager& clayManager,
	 const std::map<std::string, mappers::CultureDef>& cultureDefs) const
{
	Log(LogLevel::Info) << "-> Updating Slave Pop Cultures.";
	auto counter = 0;

	for (const auto& country: politicalManager.getCountries() | std::views::values)
	{
		std::set<std::string> cultureTraits;
		for (const auto& culture: country->getProcessedData().cultures)
		{
			if (!cultureDefs.contains(culture))
				continue;
			const auto& traits = cultureDefs.at(culture).traits;
			cultureTraits.insert(traits.begin(), traits.end());
		}
		std::optional<std::string> newSlaveCulture;
		for (const auto& trait: cultureTraits)
			if (slaveCultureMapper.getSlaveCulture(trait))
			{
				newSlaveCulture = *slaveCultureMapper.getSlaveCulture(trait);
				break;
			}
		if (!newSlaveCulture)
			continue;

		for (const auto& subState: country->getSubStates())
		{
			if (subState->getSubStatePops().getSlavePopCount() == 0)
				continue;
			const auto& stateName = subState->getHomeStateName();
			if (!clayManager.stateIsInRegion(stateName, "north_america_strategic_regions") &&
				 !clayManager.stateIsInRegion(stateName, "south_america_strategic_regions"))
				continue;

			auto newSubStatePops = subState->getSubStatePops();
			std::vector<Pop> newPops;
			for (const auto& pop: subState->getSubStatePops().getPops())
			{
				if (pop.getType() == "slaves")
				{
					Pop newPop = pop;
					newPop.setCulture(*newSlaveCulture);
					newPops.emplace_back(newPop);
					++counter;
				}
				else
				{
					newPops.emplace_back(pop);
				}
			}
			newSubStatePops.setPops(newPops);
			subState->setSubStatePops(newSubStatePops);
		}
	}
	Log(LogLevel::Info) << "<> Updated " << counter << " Slave Pops.";
}

std::map<std::string, int> V3::PopManager::getVanillaSuperRegionalPopCounts(const ClayManager& clayManager) const
{
	std::map<std::string, int> toReturn;

	for (const auto& [superRegionName, superRegion]: clayManager.getSuperRegions())
		for (const auto& region: superRegion->getRegions() | std::views::values)
			for (const auto& stateName: region->getStates() | std::views::keys)
				if (vanillaStatePops.contains(stateName))
					toReturn[superRegionName] += vanillaStatePops.at(stateName).getPopCount();

	return toReturn;
}

std::map<std::string, double> V3::PopManager::getVanillaSuperRegionalWeights(const ClayManager& clayManager)
{
	std::map<std::string, double> toReturn;

	for (const auto& [superRegionName, superRegion]: clayManager.getSuperRegions())
		toReturn[superRegionName] = superRegion->getTotalSubStateWeight();

	return toReturn;
}

std::map<std::string, double> V3::PopManager::getSuperRegionPopShapingProjections(const ClayManager& clayManager) const
{
	std::map<std::string, double> toReturn;
	for (const auto& [stateName, state]: clayManager.getStates())
	{
		if (state->isSea() || state->isLake())
			continue;
		const auto& superRegion = clayManager.getParentSuperRegion(stateName);
		if (!superRegion)
			continue;
		if (!vanillaStatePops.contains(stateName))
			continue;

		const auto stateInvestmentFactor = 1.0 + state->getInvestmentFactor();
		const auto projectedPopCount = static_cast<int>(static_cast<double>(vanillaStatePops.at(stateName).getPopCount()) * stateInvestmentFactor);
		toReturn[superRegion->getName()] += projectedPopCount;
	}
	return toReturn;
}
