#ifndef OUT_WORLD_H
#define OUT_WORLD_H
#include "Configuration.h"
#include "ConverterVersion.h"
#include "V3World/V3World.h"

namespace OUT
{

void exportWorld(const Configuration& configuration, const V3::World& world, const commonItems::ConverterVersion& converterVersion);
void exportVersion(const std::string& outputName, const commonItems::ConverterVersion& converterVersion);

} // namespace OUT

#endif // OUT_WORLD_H