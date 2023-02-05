#ifndef OUT_CULTURES_H
#define OUT_CULTURES_H
#include "CultureMapper/CultureDefinitionLoader/CultureDef.h"

namespace OUT
{
void exportCultures(const std::string& outputName, const std::map<std::string, mappers::CultureDef>& cultures);

} // namespace OUT

#endif // OUT_CULTURES_H