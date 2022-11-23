#ifndef CULTURE_MAPPER_H
#define CULTURE_MAPPER_H
#include "CultureMappingRule.h"
#include "Parser.h"

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
class CultureMapper: commonItems::parser
{
  public:
	CultureMapper() = default;

	void loadMappingRules(std::istream& theStream);
	void loadMappingRules(const std::string& fileName);

	[[nodiscard]] const auto& getMacros() const { return encounteredMacros; }


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

	std::vector<CultureMappingRule> cultureMapRules;
	std::map<std::string, std::string> encounteredMacros;
};
} // namespace mappers

#endif // CULTURE_MAPPER_H