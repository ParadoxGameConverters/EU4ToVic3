#include "ClayManager.h"
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

	// We're rolling across EU4's provinces and assigning them to chunks. We'll deal with ownership later as ownership can be shared.
	for (const auto& [spID, sourceProvince]: provinceManager.getAllProvinces())
	{
		if (processedEU4IDs.contains(spID))
			continue; // We already grabbed this province earlier in some other mapping. Skip.

		auto v3provinceIDs = provinceMapper.getV3Provinces(spID);
		if (v3provinceIDs.empty())
			continue; // skipping this chunk.

		auto eu4ProvinceIDs = provinceMapper.getEU4Provinces(v3provinceIDs.front());

		auto chunk = std::make_shared<Chunk>();
		// Shove all source provinces into the chunk.
		for (auto eu4ProvinceID: eu4ProvinceIDs)
		{
			if (provinceManager.getAllProvinces().contains(eu4ProvinceID))
			{
				chunk->sourceProvinces.emplace(eu4ProvinceID, sourceProvince);
			}
			else
			{
				// Don't panic before checking if this is a wasteland or lake.
				if (!provinceManager.isProvinceDiscarded(eu4ProvinceID))
					Log(LogLevel::Warning) << "Existing provinceMapper mapping for eu4 province " << eu4ProvinceID << " has no match in the save! Skipping.";
				else
					Log(LogLevel::Warning) << "Discarded eu4 province " << eu4ProvinceID << " skipping.";
			}
			processedEU4IDs.emplace(eu4ProvinceID);
		}

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
			chunk->provinces.emplace(v3provinceID, v3Province);

			// also mark this chunk impacting these states
			if (!chunk->states.contains(state->getName()))
				chunk->states.emplace(state->getName(), state);

			// And file.
			processedV3IDs.emplace(v3provinceID);
		}
		// If we don't have a single target province, bail on this chunk.
		if (chunk->provinces.empty())
			continue;

		// Store the chank and move on.
		chunks.push_back(chunk);
	}
	Log(LogLevel::Info) << "<> Generated " << chunks.size() << " Clay Chunks.";
}
