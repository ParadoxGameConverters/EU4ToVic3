#ifndef EU4_LOCALIZATION_LOADER
#define EU4_LOCALIZATION_LOADER
#include "ModLoader/ModLoader.h"
#include <map>
#include <optional>
#include <string>
#include <utility>

namespace EU4
{
class EU4LocalizationLoader
{
  public:
	EU4LocalizationLoader() = default;

	void loadLocalizations(const std::string& EU4Path, const Mods& mods);
	void loadLocalizations(std::istream& theStream);

	[[nodiscard]] std::optional<std::map<std::string, std::string>> getTextInEachLanguage(const std::string& key) const;
	[[nodiscard]] std::optional<std::string> getTextForKey(const std::string& key, const std::string& language) const;

  private:
	void readFromAllFilesInFolder(const std::string& folderPath);
	void readFromFile(const std::string& fileName);
	void readFromStream(std::istream& theStream);

	[[nodiscard]] static std::optional<std::string> determineLanguageForFile(const std::string& text);
	[[nodiscard]] static std::pair<std::string, std::string> determineKeyLocalisationPair(const std::string& text);
	[[nodiscard]] static std::string removeUTF8BOM(const std::string& text);

	std::map<std::string, std::map<std::string, std::string>> localizations; // key, map<language, value>
};
} // namespace EU4

#endif // EU4_LOCALIZATION_LOADER