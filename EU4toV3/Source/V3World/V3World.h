#ifndef WORLD_H
#define WORLD_H
#include "../EU4World/World.h"
#include "ClayManager/ClayManager.h"
#include "Configuration.h"
#include "CountryMapper/CountryMapper.h"
#include "PoliticalManager/PoliticalManager.h"
#include "ProvinceMapper/ProvinceMapper.h"

namespace V3
{
class World
{
  public:
	World(const Configuration& configuration, const EU4::World& sourceWorld, commonItems::ConverterVersion&& theConverterVersion);

  private:
	std::string V3Path;
	Configuration::ConfigBlock configBlock;
	commonItems::ConverterVersion converterVersion;
	std::string outputName;

	ClayManager clayManager;
	mappers::ProvinceMapper provinceMapper;
	PoliticalManager politicalManager;
	std::shared_ptr<mappers::CountryMapper> countryMapper;

	// output
	void createModFile() const;
	void output() const;
	void outputVersion() const;
	void outputStates() const;
	void outputCommonCountries() const;
	void outputLocs() const;
};

} // namespace V3

#endif // WORLD_H
