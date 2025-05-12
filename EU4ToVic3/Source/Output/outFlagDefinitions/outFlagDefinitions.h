#ifndef OUT_FLAG_DEFINITIONS_H
#define OUT_FLAG_DEFINITIONS_H
#include "PoliticalManager/PoliticalManager.h"

namespace OUT
{
void exportFlagDefinitions(const std::string& outputName, const std::map<std::string, std::shared_ptr<V3::Country>>& countries);

} // namespace OUT

#endif // OUT_CHARACTERS_H