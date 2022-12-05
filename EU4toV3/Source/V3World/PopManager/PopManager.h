#ifndef POP_MANAGER_H
#define POP_MANAGER_H
#include "ModLoader/ModFilesystem.h"
#include "Pops/StatePops.h"
#include <map>
#include <memory>
#include <string>

namespace EU4
{
class CultureLoader;
class ReligionLoader;
} // namespace EU4
namespace mappers
{
class CultureMapper;
class ReligionMapper;
} // namespace mappers
namespace V3
{
class ClayManager;
class SubState;
class PopManager
{
  public:
	PopManager() = default;

	void initializeVanillaPops(const commonItems::ModFilesystem& modFS);
	void convertDemographics(const ClayManager& clayManager,
		 const mappers::CultureMapper& cultureMapper,
		 const mappers::ReligionMapper& religionMapper,
		 const EU4::CultureLoader& cultureLoader,
		 const EU4::ReligionLoader& religionLoader) const;
	void generatePops(const ClayManager& clayManager) const;

	[[nodiscard]] std::string getDominantVanillaCulture(const std::string& stateName) const;
	[[nodiscard]] std::string getDominantVanillaReligion(const std::string& stateName) const;
	[[nodiscard]] std::optional<SubStatePops> getVanillaSubStatePops(const std::string& stateName, const std::string& ownerTag) const;

  private:
	[[nodiscard]] int generatePopCountForShovedSubState(const std::shared_ptr<SubState>& subState, int unassignedPopCount, int unassignedProvinces) const;
	[[nodiscard]] int generatePopCountForNormalSubState(const std::shared_ptr<SubState>& subState, int unassignedPopCount) const;

	std::map<std::string, StatePops> vanillaStatePops; // state, StatePops
};
} // namespace V3
#endif // POP_MANAGER_H