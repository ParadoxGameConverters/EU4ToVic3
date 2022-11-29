#ifndef POP_MANAGER_H
#define POP_MANAGER_H
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
class State;
class PopManager
{
  public:
	PopManager() = default;

	void initializeVanillaPops(const std::string& v3Path);
	void assignVanillaPopsToStates(const ClayManager& clayManager);
	void importDemographicsAndStates(const ClayManager& clayManager);
	void convertDemographics(const ClayManager& clayManager,
		 const mappers::CultureMapper& cultureMapper,
		 const mappers::ReligionMapper& religionMapper,
		 const EU4::CultureLoader& cultureLoader,
		 const EU4::ReligionLoader& religionLoader) const;

	[[nodiscard]] std::string getDominantVanillaCulture(const std::string& stateName) const;

  private:
	std::map<std::string, StatePops> vanillaStatePops; // state, StatePops
	std::map<std::string, std::shared_ptr<State>> states;
};
} // namespace V3
#endif // POP_MANAGER_H