#ifndef EU4_COLONIAL_REGION_H
#define EU4_COLONIAL_REGION_H
#include "Parser.h"
#include <set>

namespace EU4
{
class ColonialRegion: commonItems::parser
{
  public:
	explicit ColonialRegion(std::istream& theStream);

	[[nodiscard]] const auto& getProvinces() const { return provinces; }

  private:
	void registerKeys();

	std::set<int> provinces;
};
} // namespace EU4

#endif // EU4_COLONIAL_REGION_H