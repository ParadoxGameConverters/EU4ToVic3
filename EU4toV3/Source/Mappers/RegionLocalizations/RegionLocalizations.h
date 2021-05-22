#ifndef REGION_LOCALIZATIONS_H
#define REGION_LOCALIZATIONS_H

#include <map>
#include <string>
#include <optional>

namespace mappers
{
	class RegionLocalizations
	{
	public:
		RegionLocalizations();

		[[nodiscard]] std::optional<std::string> getEnglishFor(const std::string& key);
		[[nodiscard]] std::optional<std::string> getFrenchFor(const std::string& key);
		[[nodiscard]] std::optional<std::string> getSpanishFor(const std::string& key);
		[[nodiscard]] std::optional<std::string> getGermanFor(const std::string& key);

	private:
		std::map<std::string, std::string> spaLocalisations; // key, localization_text
		std::map<std::string, std::string> engLocalisations;
		std::map<std::string, std::string> gerLocalisations;
		std::map<std::string, std::string> fraLocalisations;

		std::map<std::string, std::string> chopFile(const std::string& locFile) const;
	};
}

#endif // REGION_LOCALIZATIONS_H