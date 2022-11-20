#ifndef POLITICAL_MANAGER_H
#define POLITICAL_MANAGER_H
#include "CountryDefinitionLoader/Country.h"
#include <map>
#include <memory>
#include <string>

namespace V3
{
class PoliticalManager
{
  public:
	PoliticalManager() = default;

	void initializeVanillaCountries(const std::string& V3Path);
	[[nodiscard]] const auto& getCountries() const { return countries; }

  private:
	std::map<std::string, std::shared_ptr<Country>> countries;
};
} // namespace V3
#endif // POLITICAL_MANAGER_H