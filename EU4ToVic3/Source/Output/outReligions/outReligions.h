#ifndef OUT_RELIGIONS_H
#define OUT_RELIGIONS_H
#include "ReligionMapper/ReligionDefinitionLoader/ReligionDef.h"

namespace OUT
{
void exportReligions(const std::string& outputName, const std::map<std::string, mappers::ReligionDef>& religions);

} // namespace OUT

#endif // OUT_RELIGIONS_H