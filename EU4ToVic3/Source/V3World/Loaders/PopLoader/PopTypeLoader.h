#ifndef POP_TYPE_LOADER_H
#define POP_TYPE_LOADER_H
#include "ModLoader/ModFilesystem.h"
#include "Parser.h"
#include "PopManager/Pops/PopType.h"

namespace V3
{
class PopTypeLoader: commonItems::parser
{
  public:
	PopTypeLoader() = default;

	void loadPopTypes(const commonItems::ModFilesystem& modFS);
	[[nodiscard]] const auto& getPopTypes() const { return popTypes; }

  private:
	void registerKeys();

	std::map<std::string, PopType> popTypes;
};
} // namespace V3
#endif // POP_TYPE_LOADER_H