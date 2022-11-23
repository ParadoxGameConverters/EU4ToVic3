#ifndef V3_WORLD_H
#define V3_WORLD_H
#include "ClayManager/ClayManager.h"
#include "Configuration.h"
#include "CountryMapper/CountryMapper.h"
#include "CultureMapper/CultureMapper.h"
#include "PoliticalManager/PoliticalManager.h"
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

  private:
	std::string V3Path;
	Configuration::ConfigBlock configBlock;

	ClayManager clayManager;
	PoliticalManager politicalManager;

	mappers::ProvinceMapper provinceMapper;
	mappers::ReligionMapper religionMapper;
	mappers::CultureMapper cultureMapper;
	std::shared_ptr<mappers::CountryMapper> countryMapper;
};

} // namespace V3

#endif // V3_WORLD_H
