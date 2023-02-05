#ifndef VANILLA_DIPLOMATIC_PLAY_ENTRIES_H
#define VANILLA_DIPLOMATIC_PLAY_ENTRIES_H
#include "Parser.h"

namespace V3
{
class VanillaDiplomaticPlayEntries: commonItems::parser
{
  public:
	VanillaDiplomaticPlayEntries() = default;
	explicit VanillaDiplomaticPlayEntries(std::istream& theStream);

	[[nodiscard]] const auto& getDiploPlayElements() const { return diploPlayElements; }

  private:
	void registerKeys();

	parser entryParser;
	std::string activeTag;

	std::map<std::string, std::vector<std::string>> diploPlayElements; // unparsed elements from the file, ready for direct dump.
};
} // namespace V3
#endif // VANILLA_DIPLOMATIC_PLAY_ENTRIES_H