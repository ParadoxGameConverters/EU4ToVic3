#ifndef VN_COLONIAL_MAPPER
#define VN_COLONIAL_MAPPER
#include "Parser.h"
#include "VNColonialMapping.h"
#include <ranges>

namespace mappers
{
class VNColonialMapper: commonItems::parser
{
  public:
	VNColonialMapper() = default;

	void loadMappingRules(std::istream& theStream);
	void loadMappingRules(const std::string& fileName);

	[[nodiscard]] const auto& getVNColonies() const { return VNColonies; }
	[[nodiscard]] bool isStateVNColonial(const std::string& stateName) const;
	[[nodiscard]] std::set<std::string> getVanillaOwners(const std::string& stateName) const; // A state can contain multiple colonial substates.
	[[nodiscard]] std::optional<std::string> getKeyProvince(const std::string& stateName, const std::string& ownerTag) const;
	[[nodiscard]] bool isStateDecolonizable(const std::string& stateName, const std::string& ownerTag, const std::string& currentOwner) const;

  private:
	void registerKeys();

	std::vector<VNColonialMapping> VNColonies;
};
} // namespace mappers

#endif // VN_COLONIAL_MAPPER