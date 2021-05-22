#ifndef WORLD_H
#define WORLD_H
#include "../EU4World/World.h"
#include "../EU4World/Country/EU4Country.h"
#include "../Mappers/AcceptedCultureThresholds/AcceptedCultureThresholdsMapper.h"
#include "../Mappers/AfricaReset/AfricaResetMapper.h"
#include "../Mappers/CountryFlags/CountryFlags.h"
#include "../Mappers/CountryMappings/CountryMappings.h"
#include "../Mappers/CulturalUnions/CulturalUnionMapper.h"
#include "../Mappers/CultureGroups/CultureGroups.h"
#include "../Mappers/CultureMapper/CultureMapper.h"
#include "../Mappers/DeadDefinitionMapper/DeadDefinitionMapper.h"
#include "../Mappers/FactoryTypes/FactoryTypeMapper.h"
#include "../Mappers/Geography/ClimateMapper.h"
#include "../Mappers/Geography/Continents.h"
#include "../Mappers/Geography/TerrainDataMapper.h"
#include "../Mappers/GovernmentMapper/GovernmentMapper.h"
#include "../Mappers/LeaderTraits/LeaderTraitMapper.h"
#include "../Mappers/MinorityPops/MinorityPopMapper.h"
#include "../Mappers/NavalBases/NavalBaseMapper.h"
#include "../Mappers/PartyNames/PartyNameMapper.h"
#include "../Mappers/PartyTypes/PartyTypeMapper.h"
#include "../Mappers/PortProvinces/PortProvinces.h"
#include "../Mappers/ProvinceMappings/ProvinceMapper.h"
#include "../Mappers/RegimentCosts/RegimentCostsMapper.h"
#include "../Mappers/RegionLocalizations/RegionLocalizations.h"
#include "../Mappers/ReligionMapper/ReligionMapper.h"
#include "../Mappers/ReligiousGroups/ReligiousGroups.h"
#include "../Mappers/StartingInventionMapper/StartingInventionMapper.h"
#include "../Mappers/StartingTechMapper/StartingTechMapper.h"
#include "../Mappers/StateMapper/StateMapper.h"
#include "../Mappers/TechSchools/TechSchoolMapper.h"
#include "../Mappers/Unreleasables/Unreleasables.h"
#include "../Mappers/WarGoalMapper/WarGoalMapper.h"
#include "Output/ModFile.h"
#include "UnionTagsMapper/UnionTagsMapper.h"
#include <memory>

namespace mappers
{
class VersionParser;
class TechGroupsMapper;
} // namespace mappers

namespace V2
{
class World
{
  public:
	World(const std::shared_ptr<Configuration>& theConfiguration, const EU4::World& sourceWorld,
		 const mappers::IdeaEffectMapper& ideaEffectMapper,
		 const mappers::TechGroupsMapper& techGroupsMapper,
		 const mappers::VersionParser& versionParser);

  private:
	long totalWorldPopulation = 0;
	int stateId = 0; // ID counter for generated states

	std::vector<std::pair<std::string, EU4::HistoricalEntry>> historicalData; // HoI4 export dynasty+rulers


	void output(const mappers::VersionParser& versionParser, const std::string& outputName) const;
	void createModFile(const std::string& outputName) const;
	static void outputVersion(const mappers::VersionParser& versionParser, const std::string& outputName);
	void outputHistory(const std::string& outputName) const;
	void transcribeHistoricalData();

	/*
	mappers::ProvinceMapper provinceMapper;
	mappers::Continents continentsMapper;
	mappers::CountryMappings countryMapper;
	mappers::ClimateMapper climateMapper;
	mappers::TerrainDataMapper terrainDataMapper;
	mappers::CultureMapper cultureMapper;
	mappers::CultureMapper slaveCultureMapper;
	mappers::GovernmentMapper governmentMapper;
	mappers::MinorityPopMapper minorityPopMapper;
	mappers::PartyNameMapper partyNameMapper;
	mappers::PartyTypeMapper partyTypeMapper;
	mappers::RegimentCostsMapper regimentCostsMapper;
	mappers::ReligionMapper religionMapper;
	mappers::StateMapper stateMapper;
	mappers::TechSchoolMapper techSchoolMapper;
	std::unique_ptr<mappers::CulturalUnionMapper> culturalUnionMapper;
	std::unique_ptr<mappers::CulturalUnionMapper> culturalNationalitiesMapper;
	mappers::FactoryTypeMapper factoryTypeMapper;
	mappers::Unreleasables unreleasablesMapper;
	mappers::LeaderTraitMapper leaderTraitMapper;
	mappers::NavalBaseMapper navalBaseMapper;
	mappers::PortProvinces portProvincesMapper;
	mappers::WarGoalMapper warGoalMapper;
	mappers::StartingTechMapper startingTechMapper;
	mappers::StartingInventionMapper startingInventionMapper;
	mappers::CultureGroups cultureGroupsMapper;
	mappers::ReligiousGroups religiousGroupsMapper;
	mappers::RegionLocalizations regionLocalizations;
	mappers::AfricaResetMapper africaResetMapper;
	mappers::AcceptedCultureThresholdsMapper acceptedCultureThresholdsMapper;
	mappers::DeadDefinitionMapper deadDefinitionMapper;
	ModFile modFile;
	mappers::CountryFlags countryFlags;
	mappers::UnionTagsMapper unionTagsMapper;
	*/
};

std::ostream& operator<<(std::ostream& output, const std::vector<std::pair<std::string, EU4::HistoricalEntry>>& historicalData);
} // namespace V2

#endif // WORLD_H
