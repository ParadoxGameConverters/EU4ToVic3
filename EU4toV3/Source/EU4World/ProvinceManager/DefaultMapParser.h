#ifndef DEFAULTMAP_PARSER
#define DEFAULTMAP_PARSER
#include <set>
#include "Parser.h"
#include "Mods/ModLoader.h"

namespace EU4
{
class DefaultMapParser: commonItems::parser
{
  public:
	DefaultMapParser() = default;
	void loadDefaultMap(const std::string& EU4Path, const Mods& mods);
	void loadDefaultMap(std::istream& theStream);

	[[nodiscard]] const auto& getSeaIDs() const { return seas; }
	[[nodiscard]] const auto& getRandomsIDs() const { return randoms; }
	[[nodiscard]] const auto& getLakeIDs() const { return lakes; }

  private:
	void registerKeys();

	std::set<int> seas;
	std::set<int> randoms;
	std::set<int> lakes;
};
} // namespace EU4

#endif // DEFAULTMAP_PARSER
