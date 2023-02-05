#ifndef V3_SECTOR_LOADER_H
#define V3_SECTOR_LOADER_H
#include "Parser.h"
#include "Script/AddScript.h"
#include "Script/MultiplyScript.h"

namespace V3
{
// A Sector of industry is a group of vic3 buildings with similar characteristics and distribution

class SectorLoader: commonItems::parser
{
  public:
	SectorLoader() = default;
	void loadSector(std::istream& theStream);
	void setName(const std::string& theName) { name = theName; }

	[[nodiscard]] const auto& getName() const { return name; }
	[[nodiscard]] const auto& getBuildings() const { return buildings; }
	[[nodiscard]] double calculateWeight(const Country& country) const;

  private:
	void registerKeys();
	[[nodiscard]] AddScript mergeValidAdds(const Country& country) const;
	[[nodiscard]] MultiplyScript mergeValidMults(const Country& country) const;

	std::string name;
	double weight = 0;
	std::set<std::string> buildings;
	bool industrial = false;

	std::vector<AddScript> adders;
	std::vector<MultiplyScript> multipliers;
};
} // namespace V3

#endif // V3_SECTOR_LOADER_H