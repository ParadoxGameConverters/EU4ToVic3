#include "ClayManager.h"
#include "CountryMapper/CountryMapper.h"
#include "Log.h"
#include "StateLoader/StateLoader.h"
#include "SuperRegionLoader/SuperRegionLoader.h"
#include "TerrainLoader/TerrainLoader.h"
#include <numeric>
#include <ranges>

void V3::ClayManager::initializeVanillaStates(const std::string& v3Path)
{
	Log(LogLevel::Info) << "-> Initializing Vanilla States and Provinces.";
	StateLoader stateLoader;
	stateLoader.loadStates(v3Path);
	states = stateLoader.getStates();
	for (const auto& state: states | std::views::values)
		for (const auto& provinceID: state->getProvinces() | std::views::keys)
			provincesToStates.emplace(provinceID, state);

	Log(LogLevel::Info) << "<> " << states.size() << " states loaded with " << provincesToStates.size() << " provinces inside.";
}

void V3::ClayManager::loadTerrainsIntoProvinces(const std::string& v3Path)
{
	Log(LogLevel::Info) << "-> Loading Terrains into Provinces.";
	TerrainLoader terrainLoader;
	terrainLoader.loadTerrains(v3Path);
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

void V3::ClayManager::initializeSuperRegions(const std::string& v3Path)
{
	Log(LogLevel::Info) << "-> Initializing Regions and Superregions.";
	SuperRegionLoader superRegionLoader;
	superRegionLoader.loadSuperRegions(v3Path);
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
	for (const auto& [spID, sourceProvince]: provinceManager.getAllProvinces())
	{
		if (processedEU4IDs.contains(spID))
			continue; // We already grabbed this province earlier in some other mapping. Skip.

		auto v3provinceIDs = provinceMapper.getV3Provinces(spID);
		if (v3provinceIDs.empty())
			continue; // This province is mapped into nothing. Skip.

		auto eu4ProvinceIDs = provinceMapper.getEU4Provinces(v3provinceIDs.front());

		auto chunk = std::make_shared<Chunk>();
		// Shove all source provinces into the chunk.
		for (auto eu4ProvinceID: eu4ProvinceIDs)
		{
			if (provinceManager.getAllProvinces().contains(eu4ProvinceID))
			{
				chunk->addSourceProvince(std::pair(eu4ProvinceID, provinceManager.getAllProvinces().at(eu4ProvinceID)));
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
		if (chunk->getSourceProvinces().empty())
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

			// also mark this chunk impacting these states
			if (!chunk->getStates().contains(state->getName()))
				chunk->addState(std::pair(state->getName(), state));

			// And file both ways.
			processedV3IDs.emplace(v3provinceID);
			v3Province->setChunk(chunk);
		}
		// If we don't have a single target province, bail on this chunk.
		if (chunk->getProvinces().empty())
			continue;

		// Store the chunk and move on.
		chunks.push_back(chunk);
	}
	Log(LogLevel::Info) << "<> Generated " << chunks.size() << " Clay Chunks.";
}

void V3::ClayManager::unDisputeChunkOwnership(const std::map<std::string, std::shared_ptr<EU4::Country>>& sourceCountries)
{
	Log(LogLevel::Info) << "-> Untangling chunk ownerships.";
	// raw chunks can link to sourceProvinces of several owners. Entire chunk goes to the owner with most source Development Weight.
	std::vector<std::shared_ptr<Chunk>> filteredChunks;

	for (const auto& chunk: chunks)
	{
		auto ownerWeights = calcChunkOwnerWeights(chunk);

		// did we get anything? anyone?
		if (ownerWeights.empty())
		{
			// There were no owners to the provinces (possibly sea zones or wastelands).
			// If sea zones, drop the chunk
			if (isChunkSea(chunk))
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

		chunk->setSourceOwner(sourceCountries.at(newOwner->first));
		filteredChunks.push_back(chunk);
	}
	chunks.swap(filteredChunks);
	Log(LogLevel::Info) << "<> Untangled chunk ownerships, " << chunks.size() << " of " << filteredChunks.size() << " remain.";
}

bool V3::ClayManager::isChunkSea(const std::shared_ptr<Chunk>& chunk)
{
	for (const auto& sourceProvince: chunk->getSourceProvinces() | std::views::values)
		if (sourceProvince->isSea()) // a single sea province will suffice.
			return true;
	return false;
}

std::map<std::string, double> V3::ClayManager::calcChunkOwnerWeights(const std::shared_ptr<Chunk>& chunk)
{
	std::map<std::string, double> ownerWeights; // ownerTag, total province weight
	for (const auto& sourceProvince: chunk->getSourceProvinces() | std::views::values)
	{
		const auto& sourceTag = sourceProvince->getOwnerTag();
		if (sourceTag.empty())
			continue; // not relevant source - wasteland etc.
		if (ownerWeights.contains(sourceTag))
			ownerWeights.at(sourceTag) += sourceProvince->getProvinceWeight(); // this is RAW province weight - dev + buildings.
		else
			ownerWeights.emplace(sourceTag, sourceProvince->getProvinceWeight());
	}
	return ownerWeights;
}

void V3::ClayManager::distributeChunksAcrossSubStates()
{
	Log(LogLevel::Info) << "-> Distributing Clay Chunks across Substates.";

	// Every chunk can belong to a number of substates. We'll now transfer provinces from a chunk according to
	// their geographical State and create Substates. Every substate must belong to a single owner - or no owner - same as chunks.
	//
	// Merging of same-owner substates within a single state is NOT DONE YET! We need a strict relation between a substate, its
	// originating chunk, and thus its original provinces so we may shape populations and development properly.
	//
	// Merging of unowned substates within a single state is not done yet either. (it's done only after they are populated with some
	// pops and merged according to culture - not ownership!).

	auto [tagStateProvinces, sourceOwners] = sortChunkProvincesIntoTagStates();

	// Now build substates. Substates are divorced from chunks and no sensible direct link to original eu4 provinces can remain.
	// That's why every province knows what chunk it belonged to and can work back from there.

	substates = buildSubStates(tagStateProvinces, sourceOwners);

	crossLinkSubStatesToChunks();

	Log(LogLevel::Info) << "<> Substates organized, " << substates.size() << " produced.";
}

std::pair<V3::ChunkToEU4TagToStateToProvinceMap, V3::SourceOwners> V3::ClayManager::sortChunkProvincesIntoTagStates() const
{
	SourceOwners sourceOwners;
	ChunkToEU4TagToStateToProvinceMap chunkTagProvinces;

	auto chunkCounter = 0;

	for (const auto& chunk: chunks)
	{
		++chunkCounter;

		// build the containers
		std::string ownerTag;
		if (chunk->getSourceOwner())
		{
			ownerTag = chunk->getSourceOwner()->getTag();
		}
		else
		{
			// In order to keep every unowned chunk separate and unmerged, click here to see this simple trick!
			ownerTag = "unowned" + std::to_string(chunkCounter);
		}

		if (!sourceOwners.contains(ownerTag))
			sourceOwners.emplace(ownerTag, chunk->getSourceOwner());
		chunkTagProvinces.emplace(chunkCounter, EU4TagToStateToProvinceMap());
		chunkTagProvinces.at(chunkCounter).emplace(ownerTag, StateToProvinceMap{});
		for (const auto& stateName: chunk->getStates() | std::views::keys)
			chunkTagProvinces.at(chunkCounter).at(ownerTag).emplace(stateName, ProvinceMap{});

		// and shove the provinces into baskets
		for (const auto& [provinceName, province]: chunk->getProvinces())
		{
			if (!provincesToStates.contains(provinceName))
			{
				// We should have filtered all unviable provinces already when generating chunks, but better safe than sorry.
				Log(LogLevel::Warning) << "Filtering province " << provinceName << " failed as it belongs to no state. Skipping.";
				continue;
			}
			const auto& stateName = provincesToStates.at(provinceName)->getName();
			chunkTagProvinces.at(chunkCounter).at(ownerTag).at(stateName).emplace(provinceName, province);
		}
	}
	return {chunkTagProvinces, sourceOwners};
}

std::vector<std::shared_ptr<V3::SubState>> V3::ClayManager::buildSubStates(const ChunkToEU4TagToStateToProvinceMap& chunkTagProvinces,
	 const SourceOwners& sourceOwners) const
{
	std::vector<std::shared_ptr<SubState>> subStates;

	for (const auto& tagStateProvinces: chunkTagProvinces | std::views::values)
	{
		for (const auto& [eu4tag, stateMap]: tagStateProvinces)
		{
			for (const auto& [stateName, provinces]: stateMap)
			{
				if (provinces.empty())
					continue; // Unsure how this could happen, but sure, skip this substate.

				if (!states.contains(stateName))
				{
					// wtf, should never happen.
					Log(LogLevel::Error) << "Substate owner " << eu4tag << " wants a substate in " << stateName << " which does't exist?! Bailing on this clay!";
					continue;
				}
				if (eu4tag.starts_with("unowned"))
				{
					// This will keep unlinked substates without an owner.
					subStates.push_back(std::make_shared<SubState>(states.at(stateName), nullptr, provinces));
				}
				else
				{
					// Should be ok now.
					subStates.push_back(std::make_shared<SubState>(states.at(stateName), sourceOwners.at(eu4tag), provinces));
				}
			}
		}
	}

	return subStates;
}

void V3::ClayManager::crossLinkSubStatesToChunks() const
{
	for (const auto& substate: substates)
	{
		if (substate->getProvinces().empty())
			continue;
		const auto& tokenProvince = *substate->getProvinces().begin();
		const auto& chunk = tokenProvince.second->getChunk();
		if (!chunk)
			continue;
		substate->setChunk(chunk);
		chunk->addSubState(substate);
	}
}

void V3::ClayManager::assignSubStateOwnership(const std::map<std::string, std::shared_ptr<Country>>& countries, const mappers::CountryMapper& countryMapper)
{
	Log(LogLevel::Info) << "-> Assigning substates to countries.";
	std::vector<std::shared_ptr<SubState>> filteredSubstates;

	for (const auto& substate: substates)
	{
		// unowned substates don't need assigning.
		if (!substate->getSourceOwner())
		{
			substate->getHomeState()->addSubState(substate);
			filteredSubstates.push_back(substate);
			continue;
		}

		// all the rest must have an owner and that owner must be able to map properly.
		auto eu4tag = substate->getSourceOwnerTag();

		auto v3tag = countryMapper.getV3Tag(eu4tag);
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
			Log(LogLevel::Warning) << "Substate belonging to EU4 " << eu4tag << " hasn't been mapped over? Ditching.";
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
