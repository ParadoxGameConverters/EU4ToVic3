#include "ClayManager.h"
#include "Log.h"
#include "StateLoader/StateLoader.h"
#include "SuperRegionLoader/SuperRegionLoader.h"
#include "TerrainLoader/TerrainLoader.h"
#include <numeric>
#include <ranges>

void V3::ClayManager::initializeVanillaStates(const std::string& v3Path)
{
	StateLoader stateLoader;
	stateLoader.loadStates(v3Path);
	states = stateLoader.getStates();

	const auto provinceCount = std::accumulate(states.begin(), states.end(), 0, [](int sum, const auto& state) {
		return sum + static_cast<int>(state.second->getProvinces().size());
	});
	Log(LogLevel::Info) << "> " << states.size() << " states loaded with " << provinceCount << " provinces inside.";
}

void V3::ClayManager::loadTerrainsIntoProvinces(const std::string& v3Path)
{
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
	SuperRegionLoader superRegionLoader;
	superRegionLoader.loadSuperRegions(v3Path);
	superRegions = superRegionLoader.getSuperRegions();

	const auto regionCount = std::accumulate(superRegions.begin(), superRegions.end(), 0, [](int sum, const auto& superRegion) {
		return sum + static_cast<int>(superRegion.second->getRegions().size());
	});
	Log(LogLevel::Info) << "> " << superRegions.size() << " superregions loaded with " << regionCount << " regions inside.";
}

void V3::ClayManager::loadStatesIntoSuperRegions()
{
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
