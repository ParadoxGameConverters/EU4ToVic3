#ifndef OUT_DIPLOMACY_H
#define OUT_DIPLOMACY_H
#include "PoliticalManager/PoliticalManager.h"

namespace OUT
{
void exportDiplomacy(const std::filesystem::path& outputName, const V3::PoliticalManager& politicalManager);
void exportPacts(const std::filesystem::path& outputName, const std::vector<V3::Agreement>& agreements);
void exportRelations(const std::filesystem::path& outputName, const std::map<std::string, std::shared_ptr<V3::Country>>& countries);
void exportRivals(const std::filesystem::path& outputName, const std::map<std::string, std::shared_ptr<V3::Country>>& countries);
void exportTruces(const std::filesystem::path& outputName, const std::map<std::string, std::shared_ptr<V3::Country>>& countries);
void exportPowerBlocs(const std::filesystem::path& outputName, const std::vector<V3::PowerBloc>& powerBlocs);
} // namespace OUT

#endif // OUT_DIPLOMACY_H