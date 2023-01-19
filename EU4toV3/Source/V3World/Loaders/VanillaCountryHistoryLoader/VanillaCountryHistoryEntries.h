#ifndef VANILLA_COUNTRY_HISTORY_ENTRIES_H
#define VANILLA_COUNTRY_HISTORY_ENTRIES_H
#include "Parser.h"

namespace V3
{
class VanillaCountryHistoryEntries: commonItems::parser
{
  public:
	VanillaCountryHistoryEntries() = default;
	explicit VanillaCountryHistoryEntries(std::istream& theStream);

	[[nodiscard]] const auto& getCountryHistoryElements() const { return countryHistoryElements; }

  private:
	void registerKeys();

	parser entryParser;
	std::string activeTag;

	std::map<std::string, std::vector<std::string>> countryHistoryElements; // unparsed elements from the file, ready for direct dump.
};
} // namespace V3
#endif // VANILLA_COUNTRY_HISTORY_ENTRIES_H