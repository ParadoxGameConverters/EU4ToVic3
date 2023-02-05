#ifndef V3_PRODUCTION_METHOD_H
#define V3_PRODUCTION_METHOD_H
#include "Parser.h"

namespace V3
{
class ProductionMethod: commonItems::parser
{
  public:
	ProductionMethod() = default;
	void loadProductionMethod(std::istream& theStream);
	void setName(const std::string& theName) { name = theName; }

	[[nodiscard]] const auto& getName() const { return name; }
	[[nodiscard]] const auto& getEmployment() const { return employment; }
	[[nodiscard]] const auto& getBureaucracy() const { return bureaucracy; }
	[[nodiscard]] const auto& getUnlockingTechs() const { return unlockingTechs; }

  private:
	void registerKeys();

	parser cModUnwrapper;
	parser bModUnwrapper;

	std::string name;
	std::map<std::string, int> employment;
	int bureaucracy = 0;
	std::set<std::string> unlockingTechs;
};
} // namespace V3

#endif // V3_PRODUCTION_METHOD_H