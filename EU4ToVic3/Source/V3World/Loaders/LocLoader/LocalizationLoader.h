#ifndef LOCALIZATION_LOADER
#define LOCALIZATION_LOADER
#include "ModLoader/ModFilesystem.h"
#include <map>
#include <optional>
#include <set>
#include <string>

namespace V3
{
using LocMap = std::map<std::string, std::string>; // language - loc
class LocalizationLoader
{
  public:
	LocalizationLoader() = default;
	void scrapeLocalizations(const commonItems::ModFilesystem& modFS);
	void scrapeStream(std::istream& theStream, const std::string& language);

	[[nodiscard]] std::optional<LocMap> getLocMapForKey(const std::string& key) const;
	[[nodiscard]] std::optional<std::string> getLocForKey(const std::string& key, const std::string& language) const;

  private:
	void scrapeLanguage(const std::string& language, const commonItems::ModFilesystem& modFS);

	[[nodiscard]] static std::pair<std::string, std::string> determineKeyLocalizationPair(const std::string& text);

	std::map<std::string, LocMap> localizations;

	std::set<std::string> knownLanguages;
};
} // namespace V3

#endif // LOCALIZATION_LOADER