#ifndef TECH_CONVERSION_HELPERS_H
#define TECH_CONVERSION_HELPERS_H
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace V3
{
class Country;
class TechValues
{
  public:
	explicit TechValues(const std::map<std::string, std::shared_ptr<Country>>& countries);

	[[nodiscard]] static bool isValidCountryForTechConversion(const Country& country);
	[[nodiscard]] double getProductionTechPercentile(const std::string& tag) const;
	[[nodiscard]] double getMilitaryTechPercentile(const std::string& tag) const;
	[[nodiscard]] double getSocietyTechPercentile(const std::string& tag) const;

  private:
	[[nodiscard]] static double getCountryProductionTech(const Country& country);
	[[nodiscard]] static double getCountryMilitaryTech(const Country& country);
	[[nodiscard]] static double getCountrySocietyTech(const Country& country);

	void gatherScores(const std::map<std::string, std::shared_ptr<Country>>& countries);
	void calculateOrders();
	[[nodiscard]] static std::vector<std::string> sortMap(const std::map<std::string, double>& theMap);

	std::map<std::string, double> productionScores;
	std::map<std::string, double> militaryScores;
	std::map<std::string, double> societyScores;
	std::vector<std::string> productionOrder;
	std::vector<std::string> militaryOrder;
	std::vector<std::string> societyOrder;
};
} // namespace V3

#endif // TECH_CONVERSION_HELPERS_H