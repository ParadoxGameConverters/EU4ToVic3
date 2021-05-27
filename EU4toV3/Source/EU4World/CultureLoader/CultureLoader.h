#ifndef CULTURE_LOADER
#define CULTURE_LOADER
#include "CultureGroupParser.h"
#include "Mods/ModLoader.h"
#include "Parser.h"

namespace EU4
{
class CultureLoader: commonItems::parser
{
  public:
	CultureLoader() = default;

	void loadCultures(const std::string& EU4Path, const Mods& mods);
	void loadCultures(std::istream& theStream);

	[[nodiscard]] const auto& getCultureGroupsMap() const { return cultureGroupsMap; }

  private:
	void registerKeys();

	std::map<std::string, CultureGroupParser> cultureGroupsMap;
};
} // namespace EU4

#endif // CULTURE_LOADER
