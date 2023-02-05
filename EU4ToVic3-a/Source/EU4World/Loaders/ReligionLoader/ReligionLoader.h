#ifndef RELIGION_LOADER
#define RELIGION_LOADER
#include "ModLoader/ModFilesystem.h"
#include "Parser.h"
#include "Religion.h"

namespace EU4
{
class ReligionLoader: commonItems::parser
{
  public:
	ReligionLoader() = default;

	void loadReligions(const commonItems::ModFilesystem& modFS);
	void loadReligions(std::istream& theStream);

	[[nodiscard]] const auto& getAllReligions() const { return religions; }
	[[nodiscard]] std::optional<std::string> getGroupForReligion(const std::string& religion) const;

  private:
	void registerKeys();

	std::optional<std::string> activeTrappings; // used to pass along filename metadata
	std::map<std::string, Religion> religions;
};
} // namespace EU4

#endif // RELIGION_LOADER