#ifndef PROVINCE_HISTORY_H
#define PROVINCE_HISTORY_H
#include "ConvenientParser.h"
#include "Date.h"
#include "DatingData.h"
#include "PopRatio.h"
#include <vector>

namespace EU4
{
class ProvinceHistory: commonItems::convenientParser
{
  public:
	ProvinceHistory() = default;
	explicit ProvinceHistory(std::istream& theStream);

	void setStartingCulture(const std::string& culture);
	void setStartingReligion(const std::string& religion);

	[[nodiscard]] const auto& getStartingCulture() const { return startingCulture; }
	[[nodiscard]] const auto& getStartingReligion() const { return startingReligion; }

	[[nodiscard]] bool hasInitializedHistory() const { return !religionHistory.empty() && !cultureHistory.empty(); }
	[[nodiscard]] auto getOriginalDevelopment() const { return originalTax + originalProduction + originalManpower; }

	[[nodiscard]] const auto& getOwnershipHistory() const { return ownershipHistory; }
	[[nodiscard]] const auto& getReligionHistory() const { return religionHistory; }
	[[nodiscard]] const auto& getCultureHistory() const { return cultureHistory; }

	void buildPopRatios(double assimilationFactor, const DatingData& datingData);
	void addPopRatio(const PopRatio& pop) { popRatios.emplace_back(pop); }
	void markNeoCulture(const std::string& culture);
	void purgeHistories();
	[[nodiscard]] const auto& getPopRatios() const { return popRatios; }

  private:
	void registerKeys();
	void decayPopRatios(const date& oldDate, const date& newDate, PopRatio& currentPop, double assimilationFactor);

	std::string startingCulture;
	std::string startingReligion;

	double originalTax = 0;
	double originalProduction = 0;
	double originalManpower = 0;

	std::vector<std::pair<date, std::string>> ownershipHistory;
	std::vector<std::pair<date, std::string>> religionHistory;
	std::vector<std::pair<date, std::string>> cultureHistory;

	std::vector<PopRatio> popRatios;
};
} // namespace EU4

#endif // PROVINCE_HISTORY_H