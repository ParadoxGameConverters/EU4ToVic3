#ifndef V3_PRODUCTION_METHOD_GROUP_H
#define V3_PRODUCTION_METHOD_GROUP_H
#include "ModLoader/ModFilesystem.h"
#include "Parser.h"

namespace V3
{
class ProductionMethodGroup: commonItems::parser
{
  public:
	ProductionMethodGroup() = default;
	void loadProductionMethodGroup(std::istream& theStream);
	void setName(const std::string& theName) { name = theName; }

	[[nodiscard]] const auto& getName() const { return name; }
	[[nodiscard]] const auto& getPMs() const { return PMs; }

  private:
	void registerKeys();

	std::string name;
	std::vector<std::string> PMs;
};
} // namespace V3

#endif // V3_PRODUCTION_METHOD_GROUP_H