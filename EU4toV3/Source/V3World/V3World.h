#ifndef WORLD_H
#define WORLD_H
#include "../EU4World/World.h"
#include "Output/outConverterVersion.h"
#include <memory>

namespace mappers
{
class ConverterVersion;
class TechGroupsMapper;
} // namespace mappers

namespace V2
{
class World
{
  public:
	World(const std::shared_ptr<Configuration>& theConfiguration, const EU4::World& sourceWorld, const mappers::ConverterVersion& converterVersion);

  private:

	void output(const mappers::ConverterVersion& converterVersion, const std::string& outputName) const;
	static void outputVersion(const mappers::ConverterVersion& converterVersion, const std::string& outputName);

};

} // namespace V2

#endif // WORLD_H
