#ifndef OUT_COMMON_COUNTRIES_H
#define OUT_COMMON_COUNTRIES_H
#include "Loaders/MajorFormablesLoader/MajorFormablesEntry.h"
#include "PoliticalManager/Country/Country.h"

namespace OUT
{
void exportCommonCountries(const std::filesystem::path& outputName, const std::map<std::string, std::shared_ptr<V3::Country>>& countries);
void exportHistoryCountries(const std::filesystem::path& outputName, const std::map<std::string, std::shared_ptr<V3::Country>>& countries);
void exportHistoryPopulations(const std::filesystem::path& outputName, const std::map<std::string, std::shared_ptr<V3::Country>>& countries);
void exportReleasables(const std::filesystem::path& outputName, const std::map<std::string, std::shared_ptr<V3::Country>>& countries);
void exportMajorFormables(const std::filesystem::path& outputName, const std::map<std::string, V3::MajorFormablesEntry>& formables);

} // namespace OUT

#endif // OUT_COMMON_COUNTRIES_H