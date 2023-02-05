#ifndef VANILLA_CHARACTER_ENTRIES_H
#define VANILLA_CHARACTER_ENTRIES_H
#include "Parser.h"

namespace V3
{
class VanillaCharacterEntries: commonItems::parser
{
  public:
	VanillaCharacterEntries() = default;
	explicit VanillaCharacterEntries(std::istream& theStream);

	[[nodiscard]] const auto& getCharacterElements() const { return characterElements; }

  private:
	void registerKeys();

	parser entryParser;
	std::string activeTag;

	std::map<std::string, std::vector<std::string>> characterElements; // unparsed elements from the file, ready for direct dump.
};
} // namespace V3
#endif // VANILLA_CHARACTER_ENTRIES_H