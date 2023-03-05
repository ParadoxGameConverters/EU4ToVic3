#ifndef DEFAULTMAP_PARSER
#define DEFAULTMAP_PARSER
#include "ModLoader/ModFilesystem.h"
#include "Parser.h"
#include <set>

namespace EU4
{
class DefaultMapParser: commonItems::parser
{
  public:
	DefaultMapParser() = default;
	void loadDefaultMap(const commonItems::ModFilesystem& modFS);
	void loadDefaultMap(std::istream& theStream);

	[[nodiscard]] const auto& getSeaIDs() const { return seas; }
	[[nodiscard]] const auto& getRandomsIDs() const { return randoms; }
	[[nodiscard]] const auto& getLakeIDs() const { return lakes; }

	[[nodiscard]] auto isSea(auto provinceID) const { return seas.contains(provinceID); }
	[[nodiscard]] auto isLake(auto provinceID) const { return lakes.contains(provinceID); }
	[[nodiscard]] auto isRNW(auto provinceID) const { return randoms.contains(provinceID); }

  private:
	void registerKeys();

	std::set<int> seas;
	std::set<int> randoms;
	std::set<int> lakes;
};
} // namespace EU4

#endif // DEFAULTMAP_PARSER
