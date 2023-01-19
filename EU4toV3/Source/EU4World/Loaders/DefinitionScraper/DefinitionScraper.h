#ifndef DEFINITION_SCRAPER
#define DEFINITION_SCRAPER
#include "ModLoader/ModFilesystem.h"
#include <optional>
#include <set>
#include <string>

namespace EU4
{
class DefinitionScraper
{
  public:
	void loadDefinitions(const commonItems::ModFilesystem& modFS);
	void loadDefinitions(std::istream& theStream);

	[[nodiscard]] const auto& getProvinceIDs() const { return provinceIDs; }
	[[nodiscard]] auto isValidID(auto provinceID) const { return provinceIDs.contains(provinceID); }

  private:
	void parseStream(std::istream& theStream);

	[[nodiscard]] std::optional<int> parseLine(const std::string& line);

	std::set<int> provinceIDs;
};

} // namespace EU4
#endif // DEFINITION_SCRAPER
