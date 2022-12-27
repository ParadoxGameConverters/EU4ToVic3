#ifndef V3_WORLD_H
#define V3_WORLD_H
#include "ClayManager/ClayManager.h"
#include "Configuration.h"
#include "CountryMapper/CountryMapper.h"
#include "CultureMapper/CultureMapper.h"
#include "Loaders/LocLoader/LocalizationLoader.h"
#include "PoliticalManager/PoliticalManager.h"
#include "PopManager/PopManager.h"
#include "ProvinceMapper/ProvinceMapper.h"
#include "ReligionMapper/ReligionMapper.h"

namespace EU4
{
class World;
}

namespace V3
{
class World
{
  public:
	World(const Configuration& configuration, const EU4::World& sourceWorld);

	[[nodiscard]] const auto& getClayManager() const { return clayManager; }
	[[nodiscard]] const auto& getPoliticalManager() const { return politicalManager; }
	[[nodiscard]] const auto& getCultureMapper() const { return cultureMapper; }
	[[nodiscard]] const auto& getReligionMapper() const { return religionMapper; }
	[[nodiscard]] const auto& getDatingData() const { return datingData; }

  private:
	std::string V3Path;
	Configuration::ConfigBlock configBlock;
	DatingData datingData;

	ClayManager clayManager;
	PoliticalManager politicalManager;
	PopManager popManager;
	LocalizationLoader localizationLoader;

	mappers::ProvinceMapper provinceMapper;
	mappers::ReligionMapper religionMapper;
	mappers::CultureMapper cultureMapper;
	std::shared_ptr<mappers::CountryMapper> countryMapper;
};

} // namespace V3

#endif // V3_WORLD_H
