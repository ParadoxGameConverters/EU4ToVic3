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
	[[nodiscard]] const auto& getInputs() const { return inputs; }
	[[nodiscard]] const auto& getOutputs() const { return outputs; }
	[[nodiscard]] const auto& getBureaucracy() const { return bureaucracy; }
	[[nodiscard]] const auto& getUnlockingTechs() const { return unlockingTechs; }
	[[nodiscard]] const auto& getUnlockingLaws() const { return unlockingLaws; }
	[[nodiscard]] const auto& getBlockingLaws() const { return disallowingLaws; }

  private:
	void registerKeys();

	static std::string getType(const std::string& typeString);

	parser modifiersParser;
	parser scalingParser;

	std::string name;
	std::map<std::string, int> employment;
	std::map<std::string, double> inputs;
	std::map<std::string, double> outputs;
	int bureaucracy = 0;
	std::set<std::string> unlockingTechs;
	std::set<std::string> unlockingLaws;
	std::set<std::string> disallowingLaws;
};
} // namespace V3

#endif // V3_PRODUCTION_METHOD_H