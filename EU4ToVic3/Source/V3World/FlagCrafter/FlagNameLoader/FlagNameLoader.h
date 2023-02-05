#ifndef FLAG_NAME_LOADER
#define FLAG_NAME_LOADER
#include "Parser.h"

namespace V3
{
class FlagNameLoader: commonItems::parser
{
  public:
	FlagNameLoader() = default;
	void loadKnownFlags(const std::string& folderPath);

	[[nodiscard]] const auto& getKnownFlags() const { return knownFlags; }

  private:
	void registerKeys();

	std::set<std::string> knownFlags;
};
} // namespace V3

#endif // FLAG_NAME_LOADER