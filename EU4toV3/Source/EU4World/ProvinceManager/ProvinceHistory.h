#ifndef PROVINCE_HISTORY_H
#define PROVINCE_HISTORY_H
#include "Date.h"
#include "ConvenientParser.h"
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

	[[nodiscard]] std::optional<date> getFirstOwnedDate() const;
	[[nodiscard]] bool hasOriginalCulture() const;
	[[nodiscard]] bool wasColonized() const;
	[[nodiscard]] bool hasInitializedHistory() const { return !religionHistory.empty() && !cultureHistory.empty(); }
	[[nodiscard]] auto getOriginalDevelopment() const { return originalTax + originalProduction + originalManpower; }


  private:
	void registerKeys();

	std::string startingCulture;
	std::string startingReligion;

	std::vector<std::pair<date, std::string>> ownershipHistory;
	std::vector<std::pair<date, std::string>> religionHistory;
	std::vector<std::pair<date, std::string>> cultureHistory;

	double originalTax = 0;
	double originalProduction = 0;
	double originalManpower = 0;
};
} // namespace EU4

#endif // PROVINCE_HISTORY_H