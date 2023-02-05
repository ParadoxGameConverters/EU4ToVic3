#ifndef CULTURE_MAPPING_RULE_H
#define CULTURE_MAPPING_RULE_H
#include "Parser.h"
#include <set>

namespace EU4
{
class CultureLoader;
class ReligionLoader;
} // namespace EU4

namespace V3
{
class ClayManager;
}

namespace mappers
{
class CultureMappingRule: commonItems::parser
{
  public:
	CultureMappingRule() = default;

	void loadMappingRules(std::istream& theStream);
	void loadMappingRules(const std::string& theString);

	[[nodiscard]] auto getV3Culture() const { return v3culture; }
	[[nodiscard]] auto getCultures() const { return cultures; }
	[[nodiscard]] auto getCultureGroups() const { return cultureGroups; }
	[[nodiscard]] auto getReligions() const { return religions; }
	[[nodiscard]] auto getReligionGroups() const { return religionGroups; }
	[[nodiscard]] auto getRegions() const { return regions; }
	[[nodiscard]] auto getOwners() const { return owners; }
	[[nodiscard]] auto getRequestedMacros() const { return requestedMacros; }

	[[nodiscard]] std::optional<std::string> cultureMatch(const V3::ClayManager& clayManager,
		 const EU4::CultureLoader& cultureLoader,
		 const EU4::ReligionLoader& religionLoader,
		 const std::string& eu4culture,
		 const std::string& eu4religion,
		 const std::string& v3state,
		 const std::string& v3ownerTag) const;

	[[nodiscard]] std::optional<std::string> cultureRegionalMatch(const V3::ClayManager& clayManager,
		 const EU4::CultureLoader& cultureLoader,
		 const EU4::ReligionLoader& religionLoader,
		 const std::string& eu4culture,
		 const std::string& eu4religion,
		 const std::string& v3state,
		 const std::string& v3ownerTag) const;

	[[nodiscard]] std::optional<std::string> cultureNonRegionalNonReligiousMatch(const V3::ClayManager& clayManager,
		 const EU4::CultureLoader& cultureLoader,
		 const EU4::ReligionLoader& religionLoader,
		 const std::string& eu4culture,
		 const std::string& eu4religion,
		 const std::string& v3state,
		 const std::string& v3ownerTag) const;

  private:
	void registerKeys();

	std::string v3culture;					  // vic3
	std::set<std::string> cultures;		  // eu4
	std::set<std::string> cultureGroups;  // eu4
	std::set<std::string> religions;		  // eu4
	std::set<std::string> religionGroups; // eu4
	std::set<std::string> regions;		  // vic3
	std::set<std::string> owners;			  // vic3
	// Links without a region will still match for suspicious matches looking for neocultures.
	// In practical terms, straight brazilian->brazilian link will match regardless of *where*.
	bool neocultureOverride = false;

	std::set<std::string> requestedMacros;
};
} // namespace mappers

#endif // CULTURE_MAPPING_RULE_H
