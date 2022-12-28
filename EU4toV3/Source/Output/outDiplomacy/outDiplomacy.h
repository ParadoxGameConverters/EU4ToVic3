#ifndef OUT_DIPLOMACY_H
#define OUT_DIPLOMACY_H
#include "PoliticalManager/PoliticalManager.h"

namespace OUT
{
void exportDiplomacy(const std::string& outputName, const V3::PoliticalManager& politicalManager);
void exportPacts(const std::string& outputName, const std::vector<V3::Agreement>& agreements);
void exportRelations(const std::string& outputName, const std::map<std::string, std::shared_ptr<V3::Country>>& countries);
void exportRivals(const std::string& outputName, const std::map<std::string, std::shared_ptr<V3::Country>>& countries);
void exportTruces(const std::string& outputName, const std::map<std::string, std::shared_ptr<V3::Country>>& countries);
} // namespace OUT

#endif // OUT_DIPLOMACY_H