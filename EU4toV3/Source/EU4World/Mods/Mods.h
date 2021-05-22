#ifndef EU4_MODS_H
#define EU4_MODS_H
#include "Configuration.h"
#include <map>
#include <optional>

namespace EU4
{
class Mods
{
  public:
	Mods() = default;
	explicit Mods(const std::vector<std::string>& usedMods);

	[[nodiscard]] std::optional<std::string> getModPath(const std::string& modName) const;

  private:
	void loadEU4ModDirectory();
	void loadSteamWorkshopDirectory();
	void loadCK2ExportDirectory();
	void loadModDirectory(const std::string& searchDirectory);

	[[nodiscard]] bool extractZip(const std::string& archive, const std::string& path) const;

	std::map<std::string, std::string> possibleMods;
	std::map<std::string, std::string> possibleCompressedMods;
};
} // namespace EU4

#endif // EU4_MODS_H