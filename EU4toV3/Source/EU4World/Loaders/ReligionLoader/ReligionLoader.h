#ifndef RELIGION_LOADER
#define RELIGION_LOADER
#include "ModLoader/ModLoader.h"
#include "Parser.h"

namespace EU4
{
struct Religion
{
	std::string name;
	std::string group;
	std::string trappings; // a converted_dynamic_faith_107 can have "orthodox" trappings if split from orthodox in ck3 regardless of actual group.
};

class ReligionLoader: commonItems::parser
{
  public:
	ReligionLoader() = default;

	void loadReligions(const std::string& EU4Path, const Mods& mods);
	void loadReligions(std::istream& theStream);

	[[nodiscard]] const auto& getAllReligions() const { return religions; }

  private:
	void registerKeys();

	std::optional<std::string> activeTrappings; // used to pass along filename metadata
	std::map<std::string, Religion> religions;
};
} // namespace EU4

#endif // RELIGION_LOADER