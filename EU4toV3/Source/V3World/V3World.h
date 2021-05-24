#ifndef WORLD_H
#define WORLD_H
#include "../EU4World/World.h"
#include "Output/outConverterVersion.h"
#include <memory>
#include "Output/outModFile.h"

namespace V3
{
class World
{
  public:
	World(const std::shared_ptr<Configuration>& configuration, const EU4::World& sourceWorld);

  private:

	//output
	void createModFile(const std::string& outputName) const;
	void output(const mappers::ConverterVersion& converterVersion, const std::string& outputName) const;
	static void outputVersion(const mappers::ConverterVersion& converterVersion, const std::string& outputName);

	ModFile modFile;
};

} // namespace V3

#endif // WORLD_H
