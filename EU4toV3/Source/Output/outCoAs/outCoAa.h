#ifndef OUT_COAS_H
#define OUT_COAS_H
#include "PoliticalManager/PoliticalManager.h"

namespace OUT
{
void exportCustomCoAs(const std::string& outputName, const std::map<std::string, std::shared_ptr<V3::Country>>& countries);

} // namespace OUT

#endif // OUT_COAS_H