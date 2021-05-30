#ifndef EU4_MODSLOADER_H
#define EU4_MODSLOADER_H
#include "Configuration.h"
#include <map>
#include <optional>

namespace EU4
{
typedef std::map<std::string, std::string> Mods;

class ModLoader
{
  public:
	ModLoader() = default;

	void loadMods(const Configuration& configuration, const Mods& incomingMods);

	[[nodiscard]] const auto& getMods() const { return usableMods; }

  private:
	void loadEU4ModDirectory(const Configuration& configuration, const Mods& incomingMods);

	[[nodiscard]] std::optional<std::string> getModPath(const std::string& modName) const;
	[[nodiscard]] bool extractZip(const std::string& archive, const std::string& path) const;

	Mods possibleMods;			  // name, absolute path to mod directory
	Mods possibleCompressedMods; // name, absolute path to zip file
	Mods usableMods;				  // name, absolute path for directories, relative for unpacked
};
} // namespace EU4

#endif // EU4_MODSLOADER_H