#ifndef FLAG_CRAFTER
#define FLAG_CRAFTER
#include <map>
#include <optional>
#include <set>
#include <string>

namespace V3
{
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
	[[nodiscard]] std::optional<std::map<FLAGTYPE, std::string>> getFlagsForEntity(const std::string& name);

  private:
	void loadKnownFlags(const std::string& folderPath);
	void filterKnownFlags();

	std::set<std::string> spentFlags;
	std::set<std::string> knownFlags;
	std::map<std::string, std::map<FLAGTYPE, std::string>> availableFlags;
};
} // namespace V3

#endif // FLAG_NAME_LOADER