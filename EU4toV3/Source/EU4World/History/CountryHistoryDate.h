#ifndef EU4_COUNTRY_HISTORY_DATE_H
#define EU4_COUNTRY_HISTORY_DATE_H
#include "Leader/EU4Leader.h"
#include "Parser.h"

namespace EU4
{
class CountryHistoryDate: commonItems::parser
{
  public:
	explicit CountryHistoryDate(std::istream& theStream, const std::string& leaderClass);

	[[nodiscard]] const auto& getLeaders() const { return leaders; }
	[[nodiscard]] const auto& getDynasty() const { return dynasty; }

  private:
	void registerKeys(const std::string& leaderClass);

	std::vector<Leader> leaders;
	std::string dynasty; // This is used to prep hoi4 export data with the last monarch dynasty.
};
} // namespace EU4

#endif // EU4_COUNTRY_HISTORY_DATE_H