#ifndef OUT_SECRET_GOALS_H
#define OUT_SECRET_GOALS_H
#include "PoliticalManager/Country/Country.h"

namespace OUT
{
void exportSecretGoals(const std::string& outputName, const std::map<std::string, std::shared_ptr<V3::Country>>& countries);

} // namespace OUT

#endif // OUT_SECRET_GOALS_H