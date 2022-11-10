#ifndef WORLD_H
#define WORLD_H
#include "../EU4World/World.h"
#include "Configuration.h"
#include "StateManager/StateManager.h"

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

	StateManager stateManager;

	// output
	void createModFile() const;
	void output() const;
	void outputVersion() const;
};

} // namespace V3

#endif // WORLD_H
