#ifndef V3_SECTOR_H
#define V3_SECTOR_H
#include "Parser.h"
#include "Script/AddScript.h"
#include "Script/MultiplyScript.h"

namespace V3
{
// A Sector of industry is a group of vic3 buildings with similar characteristics and distribution

class Sector: commonItems::parser
{
  public:
	Sector() = default;
	void loadSector(std::istream& theStream);
	void setName(const std::string& theName) { name = theName; }

  private:
	void registerKeys();
	void mergeModifiers();

	std::string name;
	double weight = 0;
	std::vector<std::string> buildings;

	std::vector<AddScript> adders;
	std::vector<MultiplyScript> multipliers;
};
} // namespace V3

#endif // V3_MATH_SCRIPT_H