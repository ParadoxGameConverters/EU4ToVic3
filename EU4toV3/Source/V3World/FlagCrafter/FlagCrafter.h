#ifndef FLAG_CRAFTER
#define FLAG_CRAFTER
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

	void loadAvailableFlags(const std::string& folderPath);
	void distributeAvailableFlags(const std::map<std::string, std::shared_ptr<Country>>& countries, const mappers::CountryMapper& countryMapper);

	[[nodiscard]] std::optional<std::map<FLAGTYPE, std::string>> getFlagsForEntity(const std::string& name);

  private:
	[[nodiscard]] bool tryAssigningFlagViaValue(const std::shared_ptr<Country>& country, const std::string& value);
	void loadKnownFlags(const std::string& folderPath);
	void filterKnownFlags();

	std::set<std::string> spentFlags;
	std::set<std::string> knownFlags;
	std::map<std::string, std::map<FLAGTYPE, std::string>> availableFlags;
};
} // namespace V3

#endif // FLAG_NAME_LOADER