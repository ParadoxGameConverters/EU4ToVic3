#ifndef PROVINCE_HISTORY_H
#define PROVINCE_HISTORY_H
#include "ConvenientParser.h"
#include "Date.h"
#include <optional>
#include <vector>

namespace EU4
{
class ProvinceHistory: commonItems::convenientParser
{
  public:
	ProvinceHistory() = default;
	explicit ProvinceHistory(std::istream& theStream);

	void setStartingCulture(const std::string& culture) { startingCulture = culture; }
	void setStartingReligion(const std::string& religion) { startingReligion = religion; }

	[[nodiscard]] const auto& getStartingCulture() const { return startingCulture; }
	[[nodiscard]] const auto& getStartingReligion() const { return startingReligion; }

	[[nodiscard]] bool hasInitializedHistory() const { return !religionHistory.empty() && !cultureHistory.empty(); }
	[[nodiscard]] auto getOriginalDevelopment() const { return originalTax + originalProduction + originalManpower; }

	[[nodiscard]] const auto& getOwnershipHistory() const { return ownershipHistory; }
	[[nodiscard]] const auto& getReligionHistory() const { return religionHistory; }
	[[nodiscard]] const auto& getCultureHistory() const { return cultureHistory; }

  private:
	void registerKeys();

	std::string startingCulture;
	std::string startingReligion;

	double originalTax = 0;
	double originalProduction = 0;
	double originalManpower = 0;

	std::vector<std::pair<date, std::string>> ownershipHistory;
	std::vector<std::pair<date, std::string>> religionHistory;
	std::vector<std::pair<date, std::string>> cultureHistory;
};
} // namespace EU4

#endif // PROVINCE_HISTORY_H