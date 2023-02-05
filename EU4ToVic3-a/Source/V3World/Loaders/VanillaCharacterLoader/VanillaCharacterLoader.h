#ifndef VANILLA_CHARACTER_LOADER_H
#define VANILLA_CHARACTER_LOADER_H
#include "ModLoader/ModFilesystem.h"
#include "Parser.h"

namespace V3
{
class VanillaCharacterLoader: commonItems::parser
{
  public:
	VanillaCharacterLoader() = default;
	void loadVanillaCharacters(const commonItems::ModFilesystem& modFS);

	[[nodiscard]] const auto& getCharacterElements() const { return characterElements; }

  private:
	void registerKeys();

	std::map<std::string, std::vector<std::string>> characterElements;
};
} // namespace V3
#endif // VANILLA_CHARACTER_LOADER_H