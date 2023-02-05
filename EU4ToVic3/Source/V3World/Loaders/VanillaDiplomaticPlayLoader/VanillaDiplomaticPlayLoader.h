#ifndef VANILLA_DIPLOMATIC_PLAY_LOADER_H
#define VANILLA_DIPLOMATIC_PLAY_LOADER_H
#include "ModLoader/ModFilesystem.h"
#include "Parser.h"

namespace V3
{
class VanillaDiplomaticPlayLoader: commonItems::parser
{
  public:
	VanillaDiplomaticPlayLoader() = default;
	void loadVanillaDiplomaticPlays(const commonItems::ModFilesystem& modFS);

	[[nodiscard]] const auto& getDiploPlayElements() const { return diploPlayElements; }

  private:
	void registerKeys();

	std::map<std::string, std::vector<std::string>> diploPlayElements;
};
} // namespace V3
#endif // VANILLA_DIPLOMATIC_PLAY_LOADER_H