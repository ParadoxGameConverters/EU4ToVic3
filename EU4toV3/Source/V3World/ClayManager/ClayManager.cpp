#include "ClayManager.h"
#include "CountryMapper/CountryMapper.h"
#include "Loaders/StateLoader/StateLoader.h"
#include "Loaders/SuperRegionLoader/SuperRegionLoader.h"
#include "Loaders/SuperRegionLoader/V3Region.h"
#include "Loaders/SuperRegionLoader/V3SuperRegion.h"
#include "Loaders/TerrainLoader/TerrainLoader.h"
#include "Loaders/VanillaStateLoader/VanillaStateLoader.h"
#include "Log.h"
#include "PoliticalManager/Country/Country.h"
#include "PoliticalManager/PoliticalManager.h"
#include "PopManager/PopManager.h"
#include "ProvinceManager/ProvinceManager.h"
#include "ProvinceMapper/ProvinceMapper.h"
#include "State/Chunk.h"
#include "State/Province.h"
#include "State/State.h"
#include "State/SubState.h"
#include <numeric>
#include <ranges>

void V3::ClayManager::initializeVanillaStates(const commonItems::ModFilesystem& modFS)
{
	Log(LogLevel::Info) << "-> Initializing Vanilla States and Provinces.";
	StateLoader stateLoader;
	stateLoader.loadStates(modFS);
	states = stateLoader.getStates();
	for (const auto& state: states | std::views::values)
		for (const auto& provinceID: state->getProvinces() | std::views::keys)
			provincesToStates.emplace(provinceID, state);

	Log(LogLevel::Info) << "<> " << states.size() << " states loaded with " << provincesToStates.size() << " provinces inside.";
}

void V3::ClayManager::loadTerrainsIntoProvinces(const commonItems::ModFilesystem& modFS)
{
	Log(LogLevel::Info) << "-> Loading Terrains into Provinces.";
	TerrainLoader terrainLoader;
	terrainLoader.loadTerrains(modFS);
	const auto& terrains = terrainLoader.getTerrains();
	for (const auto& state: states | std::views::values)
		for (const auto& [provinceName, province]: state->getProvinces())
			if (terrains.contains(provinceName))
			{
				const auto& terrain = terrains.at(provinceName);
				province->setTerrain(terrain);
				if (terrain == "ocean")
					province->setSea();
				else if (terrain == "lakes")
					province->setLake();
			}
			else
			{
				Log(LogLevel::Warning) << "Terrain for province " << provinceName << " cannot be found.";
			}
}

void V3::ClayManager::initializeSuperRegions(const commonItems::ModFilesystem& modFS)
{
	Log(LogLevel::Info) << "-> Initializing Regions and Superregions.";
	SuperRegionLoader superRegionLoader;
	superRegionLoader.loadSuperRegions(modFS);
	superRegions = superRegionLoader.getSuperRegions();

	const auto regionCount = std::accumulate(superRegions.begin(), superRegions.end(), 0, [](int sum, const auto& superRegion) {
		return sum + static_cast<int>(superRegion.second->getRegions().size());
	});
	Log(LogLevel::Info) << "<> " << superRegions.size() << " superregions loaded with " << regionCount << " regions inside.";
}

void V3::ClayManager::loadStatesIntoSuperRegions()
{
	Log(LogLevel::Info) << "-> Loading States into Regions.";
	for (const auto& superRegion: superRegions | std::views::values)
		for (const auto& region: superRegion->getRegions() | std::views::values)
		{
			std::map<std::string, std::shared_ptr<State>> theStates;
			for (const auto& stateName: region->getStates() | std::views::keys)
				if (states.contains(stateName))
					theStates.emplace(stateName, states.at(stateName));
				else
					Log(LogLevel::Warning) << "Attempting to assign state " << stateName << " which doesn't exist to region " << region->getName() << "!";
			region->replaceStates(theStates);
		}
}

void V3::ClayManager::generateChunks(const mappers::ProvinceMapper& provinceMapper, const EU4::ProvinceManager& provinceManager)
{
	Log(LogLevel::Info) << "-> Generating Clay Chunks.";

	std::set<int> processedEU4IDs;
	std::set<std::string> processedV3IDs;

	// We're rolling across EU4's provinces and assigning them to chunks. We'll deal with ownership later as ownership can be shared or missing.
	for (const auto& spID: provinceManager.getAllProvinces() | std::views::keys)
	{
		if (processedEU4IDs.contains(spID))
			continue; // We already grabbed this province earlier in some other mapping. Skip.

		auto v3provinceIDs = provinceMapper.getV3Provinces(spID);
		if (v3provinceIDs.empty())
			continue; // This province is mapped into nothing. Skip.

		auto eu4ProvinceIDs = provinceMapper.getEU4Provinces(v3provinceIDs.front());
		auto chunk = std::make_shared<Chunk>();

		// sum the total weight of incoming provinces.
		double totalWeight = 0;
		for (auto eu4ProvinceID: eu4ProvinceIDs)
			if (provinceManager.getAllProvinces().contains(eu4ProvinceID))
				totalWeight += provinceManager.getAllProvinces().at(eu4ProvinceID)->getProvinceWeight();

		// If all incoming provinces were wastelands, which is ok, sum is 0, and outgoing factors will also be 0,
		// which is again ok as wastelands don't carry ANY data.
		if (totalWeight == 0.0)
			totalWeight = 1;

		// Mark source provinces for data transfer.
		for (auto eu4ProvinceID: eu4ProvinceIDs)
		{
			if (provinceManager.getAllProvinces().contains(eu4ProvinceID))
			{
				const auto sourceWeight = provinceManager.getAllProvinces().at(eu4ProvinceID)->getProvinceWeight();
				chunk->addSourceProvinceData(*provinceManager.getAllProvinces().at(eu4ProvinceID), sourceWeight / totalWeight);
			}
			else if (!provinceManager.isProvinceDiscarded(eu4ProvinceID))
			{
				// Don't panic before checking if this is a lake, rnw or such. We silently skip those.
				// Whatever remains is an error, mismap or some other nonsense.
				Log(LogLevel::Warning) << "Existing provinceMapper mapping for eu4 province " << eu4ProvinceID << " has no match in the save! Skipping.";
			}
			processedEU4IDs.emplace(eu4ProvinceID);
		}
		// If no viable sources survive, bail on this chunk.
		if (chunk->getSourceProvinceData().empty())
			continue;

		// Shove all vic3 provinces into the chunk
		for (const auto& v3provinceID: v3provinceIDs)
		{
			if (processedV3IDs.contains(v3provinceID))
				continue; // We already grabbed this province earlier in some other mapping. Skip.

			if (!provincesToStates.contains(v3provinceID) || !provincesToStates.at(v3provinceID)->containsProvince(v3provinceID))
			{
				Log(LogLevel::Warning) << "Existing provinceMapper mapping for V3 province " << v3provinceID << " has no match in reality. Skipping.";
				continue;
			}
			const auto& state = provincesToStates.at(v3provinceID);
			const auto& v3Province = state->getProvince(v3provinceID);
			if (!v3Province) // ID is known but it's not initialized? This is a problem, likely due to some corruption.
			{
				processedV3IDs.emplace(v3provinceID);
				continue; // bail.
			}
			chunk->addProvince(std::pair(v3provinceID, v3Province));

			// And file.
			processedV3IDs.emplace(v3provinceID);
		}
		// If we don't have a single target province, bail on this chunk.
		if (chunk->getProvinces().empty())
			continue;

		// Store the chunk and move on.
		chunks.push_back(chunk);
	}
	Log(LogLevel::Info) << "<> Generated " << chunks.size() << " Clay Chunks.";
}

void V3::ClayManager::unDisputeChunkOwnership(const SourceOwners& sourceCountries)
{
	Log(LogLevel::Info) << "-> Untangling chunk ownerships.";

	// Raw chunks can link to sourceProvinces of several owners. Entire chunk goes to the owner with most source Development Weight.
	// We'd let the chunks do this themselves, but we want to immediately filter out all sea chunks and random corruption.

	std::vector<std::shared_ptr<Chunk>> filteredChunks;

	for (const auto& chunk: chunks)
	{
		auto ownerWeights = chunk->calcOwnerWeights();

		// did we get anything? anyone?
		if (ownerWeights.empty())
		{
			// There were no owners to the provinces (possibly sea zones or wastelands).
			// If sea zones, drop the chunk
			if (chunk->isSea())
				continue;

			// Otherwise, we have an unowned chunk. Technically, unowned chunk doesn't have disputed ownership. Success!
			filteredChunks.push_back(chunk);
			continue;
		}

		const auto newOwner = std::ranges::max_element(ownerWeights, [](std::pair<std::string, double> a, std::pair<std::string, double> b) {
			return a.second < b.second;
		});

		// Does that owner even exist?
		if (!sourceCountries.contains(newOwner->first))
		{
			// Interesting! Resolve dispute by making it unowned.
			filteredChunks.push_back(chunk);
			Log(LogLevel::Warning) << "Chunk owner " << newOwner->first << " is invalid. Dropping chunk ownership.";
			continue;
		}
		// Sanity check
		if (!sourceCountries.at(newOwner->first))
		{
			filteredChunks.push_back(chunk);
			Log(LogLevel::Warning) << "Chunk owner " << newOwner->first << " is not initialized. Dropping chunk ownership.";
			continue;
		}

		chunk->setSourceOwnerTag(newOwner->first);
		filteredChunks.push_back(chunk);
	}
	chunks.swap(filteredChunks);
	Log(LogLevel::Info) << "<> Untangled chunk ownerships, " << chunks.size() << " of " << filteredChunks.size() << " remain.";
}

void V3::ClayManager::splitChunksIntoSubStates()
{
	Log(LogLevel::Info) << "-> Distributing Clay across Substates.";

	// Every chunk can map to to a number of substates. We'll now transfer provinces and sourceProvince metadata from a chunk according
	// to their geographical State and create substates. Every substate must belong to a single owner - or no owner - same as chunks.
	// Weighted sourceProvince data is also copied and *further* weighted, according to the size of substates in regrads to the original
	// chunk... and potentially other factors.

	for (const auto& chunk: chunks)
	{
		// split chunk into substates
		auto generatedSubStates = chunkToSubStatesTransferFunction(chunk);

		// and file them.
		substates.insert(substates.end(), generatedSubStates.begin(), generatedSubStates.end());
	}

	Log(LogLevel::Info) << "<> Substates organized, " << substates.size() << " produced.";
}

std::vector<std::shared_ptr<V3::SubState>> V3::ClayManager::chunkToSubStatesTransferFunction(const std::shared_ptr<Chunk>& chunk) const
{
	/*
	 * This function uses several factors, all of which are debatable and moddable later. Of note is:
	 * subStateSizeFactor - how much of a chunk's original metadata (dev) the substate takes when it's split
	 * TODO: This should be modded by something more intelligent than raw province count comparison.
	 */

	// prep info
	const double totalChunkWeight = chunk->getTotalSourceProvinceWeight();
	const double provincesCount = static_cast<double>(chunk->getProvinces().size());
	const auto sortedProvinces = sortChunkProvincesIntoStates(chunk);

	// build substates
	auto generatedSubStates = buildSubStates(sortedProvinces);

	// update substates with metadata
	for (const auto& subState: generatedSubStates)
	{
		// for now, this is the simplest we can do. substateFactor is literally the amount of provinces / total provinces
		// TODO: REFINE LATER WITH PRIME LAND, IMPASSABLES AND WHATEVER.
		const double subStateSizeFactor = static_cast<double>(subState->getProvinces().size()) / provincesCount;

		// substate weight is an *outwardly* factor, when comparing the impact of that substate's metadata against all other
		// substates in the same state.
		const double subStateWeight = totalChunkWeight * subStateSizeFactor;

		// This here is an *inwardly* factor - we scale chunk's metadata according to the size of our substate, so we'd receive
		// fewer factories etc.
		std::vector<std::pair<SourceProvinceData, double>> additionallyWeightedSourceProvinceData;
		for (const auto& [data, weight]: chunk->getSourceProvinceData())
			additionallyWeightedSourceProvinceData.push_back(std::pair(data, weight * subStateSizeFactor));

		// file!
		subState->setWeight(subStateWeight);
		subState->setSourceProvinceData(additionallyWeightedSourceProvinceData);
		if (chunk->getSourceOwnerTag())
			subState->setSourceOwnerTag(*chunk->getSourceOwnerTag());
	}
	return generatedSubStates;
}

V3::StateToProvinceMap V3::ClayManager::sortChunkProvincesIntoStates(const std::shared_ptr<Chunk>& chunk) const
{

	StateToProvinceMap stateProvinces;

	// shove the provinces into baskets
	for (const auto& [provinceName, province]: chunk->getProvinces())
	{
		if (!provincesToStates.contains(provinceName))
		{
			// We should have filtered all unviable provinces already when generating chunks, but better safe than sorry.
			Log(LogLevel::Warning) << "Filtering province " << provinceName << " failed as it belongs to no state. Skipping.";
			continue;
		}
		const auto& stateName = provincesToStates.at(provinceName)->getName();
		if (!stateProvinces.contains(stateName))
			stateProvinces.emplace(stateName, ProvinceMap{});
		stateProvinces.at(stateName).emplace(provinceName, province);
	}
	return stateProvinces;
}

std::vector<std::shared_ptr<V3::SubState>> V3::ClayManager::buildSubStates(const StateToProvinceMap& stateProvinceMap) const
{
	std::vector<std::shared_ptr<SubState>> subStates;

	for (const auto& [stateName, provinces]: stateProvinceMap)
	{
		if (provinces.empty())
			continue; // Unsure how this could happen, but sure, skip this substate.

		if (!states.contains(stateName))
		{
			// wtf, should never happen.
			Log(LogLevel::Error) << "Attempting to create a substate in " << stateName << " which does't exist?! Bailing on this clay!";
			continue;
		}
		// Should be ok now.
		subStates.push_back(std::make_shared<SubState>(states.at(stateName), provinces));
	}

	return subStates;
}

void V3::ClayManager::assignSubStateOwnership(const std::map<std::string, std::shared_ptr<Country>>& countries, const mappers::CountryMapper& countryMapper)
{
	Log(LogLevel::Info) << "-> Assigning substates to countries.";
	std::vector<std::shared_ptr<SubState>> filteredSubstates;

	for (const auto& substate: substates)
	{
		// unowned substates are IRRELEVANT. We'll be filling up their spots with imported data, pops and buildings. DROP.
		if (!substate->getSourceOwnerTag())
			continue;

		// all the rest must have an owner and that owner must be able to map properly.
		auto eu4tag = substate->getSourceOwnerTag();
		if (eu4tag->empty())
		{
			Log(LogLevel::Warning) << "Substate belonging to EU4 country which we know nothing about? Ditching.";
			continue;
		}

		auto v3tag = countryMapper.getV3Tag(*eu4tag);
		if (v3tag && countries.contains(*v3tag))
		{
			const auto& owner = countries.at(*v3tag);
			substate->setOwner(owner);
			owner->addSubState(substate);
			filteredSubstates.push_back(substate);
			substate->getHomeState()->addSubState(substate);
		}
		else
		{
			// We're ditching substates of countries we haven't imported. Unsure how that'd happen but ok.
			Log(LogLevel::Warning) << "Substate belonging to EU4 " << *eu4tag << " hasn't been mapped over? Ditching.";
		}
	}

	substates.swap(filteredSubstates);

	Log(LogLevel::Info) << "<> " << filteredSubstates.size() << " substates assigned. " << filteredSubstates.size() - substates.size() << " substates ditched.";
}

bool V3::ClayManager::regionIsValid(const std::string& region) const
{
	if (states.contains(region))
		return true;
	if (superRegions.contains(region))
		return true;
	for (const auto& superRegion: superRegions | std::views::values)
		if (superRegion->getRegions().contains(region))
			return true;

	return false;
}

bool V3::ClayManager::stateIsInRegion(const std::string& state, const std::string& region) const
{
	// "region" can be anything - state, region or even supergroup.
	// Are we pinging self, and we are valid?
	if (state == region && regionIsValid(state))
		return true;

	// is region some *other* state?
	if (states.contains(region))
		return false;

	// let's ask supergroups and groups
	for (const auto& [superGroupName, superGroup]: superRegions)
		for (const auto& [regionName, theRegion]: superGroup->getRegions())
			if (theRegion->containsState(state) && (regionName == region || superGroupName == region))
				return true;

	return false;
}

void V3::ClayManager::injectVanillaSubStates(const commonItems::ModFilesystem& modFS, const PoliticalManager& politicalManager, const PopManager& popManager)
{
	Log(LogLevel::Info) << "-> Injecting Vanilla substates into conversion map.";
	auto subCounter = substates.size();

	VanillaStateLoader loader;
	loader.loadVanillaStates(modFS);

	for (const auto& [stateName, state]: states)
	{
		// do we need to do anything?
		if (!state->hasUnassignedProvinces())
			continue;

		// just sanity.
		auto unassignedProvinces = state->getUnassignedProvinces();
		if (unassignedProvinces.empty())
			continue;

		// grab vanilla state.
		if (!loader.getStates().contains(stateName))
		{
			// silently skip seas and lakes.
			if (state->isSea() || state->isLake())
				continue;

			Log(LogLevel::Warning) << "ModFS has no state " << stateName << ", not importing substates!";
			continue;
		}

		const auto& vanillaStateEntry = loader.getStates().at(stateName);
		const auto success = importVanillaSubStates(stateName, vanillaStateEntry, unassignedProvinces, politicalManager, popManager);

		// If we imported anything, we should also copy any potential homelands. Unsure whom they belong to, but they surely won't do harm.
		// What could possibly go wrong?
		if (success)
			for (const auto& homeland: vanillaStateEntry.getHomelands())
				state->addHomeland(homeland);
	}

	subCounter = substates.size() - subCounter;
	Log(LogLevel::Info) << "<> Imported " << subCounter << " new substates.";
}

bool V3::ClayManager::importVanillaSubStates(const std::string& stateName,
	 const VanillaStateEntry& entry,
	 const ProvinceMap& unassignedProvinces,
	 const PoliticalManager& politicalManager,
	 const PopManager& popManager)
{
	bool action = false;
	for (const auto& subStateEntry: entry.getSubStates())
	{
		const auto& ownerTag = subStateEntry.getOwnerTag();
		if (ownerTag.empty())
			continue;

		// We have a substate owner. Is he vanilla-decentralized?
		if (!politicalManager.isTagDecentralized(ownerTag))
			continue;

		// Now we have a state we can work with. Not all of its provinces are available!
		ProvinceMap availableProvinces;
		for (const auto& provinceID: subStateEntry.getProvinces())
			if (unassignedProvinces.contains(provinceID))
				availableProvinces.emplace(provinceID, unassignedProvinces.at(provinceID));

		// Anything to work with?
		if (availableProvinces.empty())
			continue;

		const auto& owner = politicalManager.getCountry(ownerTag);
		const auto& homeState = states.at(stateName);

		// form new substate.
		auto newSubState = std::make_shared<SubState>();
		newSubState->setOwner(owner);
		newSubState->setProvinces(availableProvinces);
		newSubState->setSubStateType(subStateEntry.getSubStateType());
		newSubState->setHomeState(homeState);

		// How many provinces did we lose in the transfer? Ie. How many of this substate's original provinces were already assigned to some other substate?
		// We can use this ratio to cut our popcount. Or we could use any other more involved function.
		// TODO: Use any other more involved function.
		const double subStateRatio = static_cast<double>(availableProvinces.size()) / static_cast<double>(subStateEntry.getProvinces().size());
		auto newPops = prepareInjectedSubStatePops(newSubState, subStateRatio, popManager);

		// and shove.
		newSubState->setSubStatePops(newPops);

		// and register.
		homeState->addSubState(newSubState);
		owner->addSubState(newSubState);
		substates.emplace_back(newSubState);
		action = true;
	}
	return action;
}

V3::SubStatePops V3::ClayManager::prepareInjectedSubStatePops(const std::shared_ptr<SubState>& subState,
	 double subStateRatio,
	 const PopManager& popManager) const
{
	// get its existing pops from vanilla
	auto subStatePops = popManager.getVanillaSubStatePops(subState->getHomeStateName(), *subState->getOwnerTag());
	if (!subStatePops)
	{
		Log(LogLevel::Warning) << "Substate for " << *subState->getOwnerTag() << " in " << subState->getHomeStateName() << " had no vanilla pops! Not importing!";
		return SubStatePops();
	}

	auto importedPops = subStatePops->getPops();

	// scale the pops.
	for (auto& pop: importedPops)
	{
		const double newSize = round(static_cast<double>(pop.getSize()) * subStateRatio);
		pop.setSize(static_cast<int>(newSize));
	}
	subStatePops->setPops(importedPops);

	return *subStatePops;
}

void V3::ClayManager::shoveRemainingProvincesIntoSubStates()
{
	Log(LogLevel::Info) << "-> Shoving remaining provinces into substates.";
	auto subCounter = substates.size();

	for (const auto& [stateName, state]: states)
	{
		// do we need to do anything?
		if (!state->hasUnassignedProvinces())
			continue;

		// just sanity.
		auto unassignedProvinces = state->getUnassignedProvinces();
		if (unassignedProvinces.empty())
			continue;

		// silently skip seas and lakes.
		if (state->isSea() || state->isLake())
			continue;

		makeSubStateFromProvinces(stateName, unassignedProvinces);
	}

	subCounter = substates.size() - subCounter;
	Log(LogLevel::Info) << "<> Generated " << subCounter << " new substates.";
}

void V3::ClayManager::makeSubStateFromProvinces(const std::string& stateName, const ProvinceMap& unassignedProvinces)
{
	const auto& homeState = states.at(stateName);

	auto newSubState = std::make_shared<SubState>();
	newSubState->setProvinces(unassignedProvinces);
	newSubState->setHomeState(homeState);

	homeState->addSubState(newSubState);
	substates.emplace_back(newSubState);
}
