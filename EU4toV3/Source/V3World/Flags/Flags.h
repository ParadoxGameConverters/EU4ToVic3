#ifndef FLAGS_H
#define FLAGS_H
#include "ColonialFlags/ColonialFlagsMapper.h"
#include "CountryMappings/CountryMappings.h"
#include "EU4World/Country/EU4NationalSymbol.h"
#include "FlagColors/FlagColorMapper.h"
#include <map>
#include <set>
#include <string>
#include <vector>

namespace V2
{
class Country;
class Flags
{
  public:
	void setV2Tags(const std::map<std::string, std::shared_ptr<Country>>& V2Countries, const mappers::CountryMappings& countryMapper);
	void output() const;

  private:
	void determineUseableFlags();
	static std::set<std::string> determineAvailableFlags();
	void getRequiredTags(const std::map<std::string, std::shared_ptr<Country>>& V2Countries);
	void mapTrivialTags();

	static void createOutputFolders();
	void copyFlags() const;
	void createCustomFlags() const;
	void createColonialFlags() const;

	std::set<std::string> usableFlagTags;
	std::set<std::string> requiredTags;

	std::map<std::string, std::string> tagMap;
	std::map<std::string, mappers::ColonialFlag> colonialFlagMapping; // tag, {base,overlordtag}
	std::map<std::string, EU4::CustomColors> customFlagMapping;

	mappers::ColonialFlagsMapper colonialFlagsMapper;
	mappers::FlagColorMapper flagColorMapper;

	std::vector<std::string> flagFileSuffixes = {".tga", "_communist.tga", "_fascist.tga", "_monarchy.tga", "_republic.tga"};
};
} // namespace V2
#endif // FLAGS_H