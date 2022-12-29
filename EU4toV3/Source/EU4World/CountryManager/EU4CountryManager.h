#ifndef EU4_COUNTRY_MANAGER
#define EU4_COUNTRY_MANAGER
#include "CommonCountryLoader/CommonCountryLoader.h"
#include "Configuration.h"
#include "EU4Country.h"
#include "LocalizationLoader/EU4LocalizationLoader.h"
#include "NationMergeMapper/NationMergeMapper.h"
#include "Parser.h"
#include "ProvinceManager/ProvinceManager.h"
#include "UnitTypeLoader/UnitTypeLoader.h"
#include <map>
#include <memory>

namespace EU4
{
class CountryManager: public commonItems::parser
{
  public:
	CountryManager() = default;

	void loadCountries(std::istream& theStream);

	[[nodiscard]] const auto& getCountries() const { return countries; }
	[[nodiscard]] std::shared_ptr<Country> getCountry(const std::string& tag) const;
	[[nodiscard]] const auto& getLocalizationLoader() const { return localizationLoader; }

	// the prep
	void loadUnitTypes(const std::string& EU4Path, const Mods& mods);
	void loadCommonCountries(const std::string& EU4Path, const Mods& mods);
	void loadLocalizations(const std::string& EU4Path, const Mods& mods);
	void loadLocalizations(const EU4LocalizationLoader& loader) { localizationLoader = loader; }
	void bootNationMergeMapper() { nationMergeMapper.loadNationMerge(); }
	void loadNationMergeMapper(const mappers::NationMergeMapper& mapper) { nationMergeMapper = mapper; }

	// the weave
	void updateUnitTypes();
	void linkProvincesToCountries(const ProvinceManager& provinceManager);
	void setHREAndEmperors(const std::string& HREmperor, const std::string& celestialEmperor, const ProvinceManager& provinceManager);
	void setRevolutionTarget(const std::string& revolutionTarget);
	void buildManufactoryCounts() const;
	void injectColorsIntoCountries();
	void injectLocalizations();
	void mergeNations();
	void filterDeadNations(Configuration::DEADCORES toggle);

  private:
	void registerKeys();
	void uniteJapan();
	void removeEmptyNations();
	void removeDeadLandlessNations();
	void removeLandlessNations();

	std::map<std::string, std::shared_ptr<Country>> countries;

	UnitTypeLoader unitTypeLoader;
	CommonCountryLoader commonCountryLoader;
	mappers::NationMergeMapper nationMergeMapper;
	EU4LocalizationLoader localizationLoader;
};
} // namespace EU4

#endif // EU4_COUNTRY_MANAGER