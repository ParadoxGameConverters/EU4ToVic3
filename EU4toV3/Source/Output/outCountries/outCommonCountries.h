#ifndef OUT_COMMON_COUNTRIES_H
#define OUT_COMMON_COUNTRIES_H
#include "PoliticalManager/Country/Country.h"

namespace OUT
{
void exportCommonCountries(const std::string& outputName, const std::map<std::string, std::shared_ptr<V3::Country>>& countries);

} // namespace OUT

#endif // OUT_COMMON_COUNTRIES_H