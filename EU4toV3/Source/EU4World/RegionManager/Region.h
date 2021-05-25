#ifndef EU4_REGION_H
#define EU4_REGION_H
#include "Area.h"
#include "Parser.h"

namespace EU4
{
class Region: commonItems::parser
{
  public:
	Region() = default;
	explicit Region(std::istream& theStream);

	[[nodiscard]] const auto& getAreas() const { return areas; }
	[[nodiscard]] bool regionContainsProvince(int provinceID) const;

	void linkArea(const std::pair<std::string, std::shared_ptr<Area>>& theArea) { areas.at(theArea.first) = theArea.second; }

  private:
	void registerKeys();

	std::map<std::string, std::shared_ptr<Area>> areas;
};
} // namespace EU4

#endif // EU4_REGION_H