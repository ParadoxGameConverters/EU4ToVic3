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
	void loadMods(Configuration& configuration);

	[[nodiscard]] const auto& getMods() const { return usableMods; }

  private:
	void loadEU4ModDirectory(const Configuration& configuration);

	[[nodiscard]] std::optional<std::string> getModPath(const std::string& modName) const;
	[[nodiscard]] bool extractZip(const std::string& archive, const std::string& path) const;

	std::map<std::string, std::string> possibleMods;			  // name, absolute path to mod directory
	std::map<std::string, std::string> possibleCompressedMods; // name, absolute path to zip file
	std::map<std::string, std::string> usableMods;				  // name, absolute path for directories, relative for unpacked
};
} // namespace EU4

#endif // EU4_MODS_H