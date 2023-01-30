#ifndef TECH_LOADER_H
#define TECH_LOADER_H
#include "ModLoader/ModFilesystem.h"
#include "Parser.h"
#include "Tech.h"

namespace V3
{
class TechLoader: commonItems::parser
{
  public:
	TechLoader() = default;
	void loadTechs(const commonItems::ModFilesystem& modFS);

	[[nodiscard]] const auto& getTechs() const { return techs; }

  private:
	void registerKeys();

	std::map<std::string, Tech> techs;
};
} // namespace V3
#endif // TECH_LOADER_H