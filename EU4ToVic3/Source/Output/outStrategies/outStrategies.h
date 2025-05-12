#ifndef OUT_STRATEGIES_H
#define OUT_STRATEGIES_H
#include "PoliticalManager/Country/Country.h"

namespace OUT
{
void exportStrategies(const std::string& outputName, const std::map<std::string, std::shared_ptr<V3::Country>>& countries);

} // namespace OUT

#endif // OUT_STRATEGIES_H