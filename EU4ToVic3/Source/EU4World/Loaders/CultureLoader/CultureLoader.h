#ifndef CULTURE_LOADER
#define CULTURE_LOADER
#include "CultureGroupParser.h"
#include "ModLoader/ModFilesystem.h"
#include "Parser.h"

namespace EU4
{
class CultureLoader: commonItems::parser
{
  public:
	CultureLoader() = default;

	void loadCultures(const commonItems::ModFilesystem& modFS);
	void loadCultures(std::istream& theStream);

	[[nodiscard]] const auto& getCultureGroupsMap() const { return cultureGroupsMap; }
	[[nodiscard]] std::optional<std::string> getGroupNameForCulture(const std::string& culture) const;
	[[nodiscard]] std::optional<CultureGroupParser> getGroupForCulture(const std::string& culture) const;
	[[nodiscard]] bool containsCulture(const std::string& culture) const;

  private:
	void registerKeys();

	std::set<std::string> knownCultures; // used to discard excess definitions when parsing modded sources.
	std::map<std::string, CultureGroupParser> cultureGroupsMap;
};
} // namespace EU4

#endif // CULTURE_LOADER
