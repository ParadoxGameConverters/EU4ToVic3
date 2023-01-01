#ifndef FLAG_CRAFTER
#define FLAG_CRAFTER
#include "FlagColorLoader/FlagColorLoader.h"
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>

namespace mappers
{
class CountryMapper;
}
namespace V3
{
class Country;
class FlagCrafter
{
  public:
	enum FLAGTYPE
	{
		Default,
		Republic,
		Monarchy,
		Fascist,
		Communist
	};

	FlagCrafter() = default;

	void loadAvailableFlags(const std::string& blankModPath, const std::string& vanillaPath);
	void loadCustomColors(const std::string& filePath);
	void distributeAvailableFlags(const std::map<std::string, std::shared_ptr<Country>>& countries, const mappers::CountryMapper& countryMapper);

	[[nodiscard]] std::optional<std::map<FLAGTYPE, std::string>> getFlagsForEntity(const std::string& name);

  private:
	[[nodiscard]] bool tryAssigningFlagViaValue(const std::shared_ptr<Country>& country, const std::string& value);
	void loadKnownFlags(const std::string& blankModPath, const std::string& vanillaPath);
	void filterKnownFlags();

	FlagColorLoader flagColorLoader;

	std::set<std::string> spentFlags;
	std::set<std::string> knownFlags;
	std::set<std::string> knownVanillaFlags;
	std::map<std::string, std::map<FLAGTYPE, std::string>> availableFlags;
};
} // namespace V3

#endif // FLAG_NAME_LOADER