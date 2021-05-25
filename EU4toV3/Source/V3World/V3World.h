#ifndef WORLD_H
#define WORLD_H
#include "../EU4World/World.h"
#include "Configuration.h"
#include "Output/outConverterVersion.h"

namespace V3
{
class World
{
  public:
	World(const Configuration& configuration, const EU4::World& sourceWorld, mappers::ConverterVersion&& theConverterVersion);

  private:
	std::string V3Path;
	Configuration::ConfigBlock configBlock;
	mappers::ConverterVersion converterVersion;
	std::string outputName;

	// output
	void createModFile() const;
	void output() const;
	void outputVersion() const;
};

} // namespace V3

#endif // WORLD_H
