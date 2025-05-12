#ifndef FLAG_CRAFTER
#define FLAG_CRAFTER
#include "FlagColorLoader/FlagColorLoader.h"
#include "ModLoader/ModFilesystem.h"
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
	void distributeAvailableFlags(const std::map<std::string, std::shared_ptr<Country>>& countries,
		 const mappers::CountryMapper& countryMapper,
		 const commonItems::ModFilesystem& eu4ModFS);

	[[nodiscard]] std::optional<std::map<FLAGTYPE, std::string>> getFlagsForEntity(const std::string& name);

  private:
	[[nodiscard]] bool tryAssigningFlagViaValue(const std::shared_ptr<Country>& country, const std::string& value);
	[[nodiscard]] bool tryAssigningEU4Flag(const std::shared_ptr<Country>& country, const commonItems::ModFilesystem& eu4ModFS);
	void loadKnownFlags(const std::string& blankModPath, const std::string& vanillaPath);
	void filterKnownFlags();
	void craftCustomFlag(const std::shared_ptr<Country>& country);

	// FLAGUTILS
	[[nodiscard]] static bool createCustomFlag(const commonItems::Color& c1,
		 const commonItems::Color& c2,
		 const commonItems::Color& c3,
		 const std::string& emblemPath,
		 const std::string& basePath,
		 const std::string& targetPath);

	FlagColorLoader flagColorLoader;

	std::set<std::string> spentFlags;
	std::set<std::string> knownFlags;
	std::set<std::string> knownVanillaFlags;
	std::map<std::string, std::map<FLAGTYPE, std::string>> availableFlags;

	std::vector<std::string> flagFileSuffixes = {"", "_communist", "_fascist", "_monarchy", "_republic"};
};
} // namespace V3

#endif // FLAG_NAME_LOADER