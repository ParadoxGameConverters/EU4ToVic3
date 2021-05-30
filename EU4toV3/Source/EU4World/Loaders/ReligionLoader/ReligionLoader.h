#ifndef RELIGION_LOADER
#define RELIGION_LOADER
#include "Parser.h"
#include <set>
#include "ModLoader/ModLoader.h"

namespace EU4
{
class ReligionLoader: commonItems::parser
{
  public:
	ReligionLoader() = default;

	void loadReligions(const std::string& EU4Path, const Mods& mods);
	void loadReligions(std::istream& theStream);

	[[nodiscard]] const auto& getAllReligions() const { return theReligions; }

  private:
	void registerKeys();

	std::set<std::string> theReligions;
};
} // namespace EU4

#endif // RELIGION_LOADER