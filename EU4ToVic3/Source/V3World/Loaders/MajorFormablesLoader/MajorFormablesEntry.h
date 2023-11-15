#ifndef MAJOR_FORMABLES_ENTRY_H
#define MAJOR_FORMABLES_ENTRY_H
#include "Parser.h"

namespace V3
{
class MajorFormablesEntry: commonItems::parser
{
  public:
	MajorFormablesEntry() = default;
	explicit MajorFormablesEntry(std::istream& theStream);

	[[nodiscard]] const auto& getStanzas() const { return stanzas; }
	[[nodiscard]] const auto& getPossibleStanzas() const { return possibleStanzas; }
	[[nodiscard]] const auto& getCultures() const { return cultures; }
	[[nodiscard]] const auto& getRequiredTechnology() const { return requiredTechnology; }
	void addCulture(const std::string& culture) { cultures.emplace(culture); }

  private:
	void registerKeys();

	parser possibleParser;
	parser anyCountryParser;
	parser orParser;
	parser filterParser;
	std::string activeTag;

	std::vector<std::string> stanzas;			// unparsed stanzas.
	std::vector<std::string> possibleStanzas; // unparsed stanzas in possible block.
	std::set<std::string> cultures;
	std::string requiredTechnology;
};
} // namespace V3
#endif // MAJOR_FORMABLES_ENTRY_H