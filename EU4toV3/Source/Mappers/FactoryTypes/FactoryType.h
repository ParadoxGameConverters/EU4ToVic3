#ifndef FACTORY_TYPE_H
#define FACTORY_TYPE_H
#include "Parser.h"

namespace mappers
{
class FactoryType: commonItems::parser
{
  public:
	FactoryType() = default;
	explicit FactoryType(std::istream& theStream, std::string theName);

	[[nodiscard]] auto isCoastal() const { return coastal; }
	[[nodiscard]] auto getStartingCount() const { return startingCount; }
	[[nodiscard]] const auto& getRequiredTech() const { return requiredTech; }
	[[nodiscard]] const auto& getRequiredInvention() const { return requiredInvention; }
	[[nodiscard]] const auto& getInputs() const { return inputs; }
	[[nodiscard]] const auto& getOutputs() const { return outputs; }
	[[nodiscard]] const auto& getName() const { return factoryTypeName; }

	void assignTechsAndCounts(const std::optional<std::string>& theRequiredTech,
		 const std::optional<std::string>& theRequiredInvention,
		 const std::optional<int>& theStartingCount);

  private:
	void registerKeys();

	bool coastal = false;
	int startingCount = 0;
	std::string factoryTypeName;
	std::string requiredTech;
	std::string requiredInvention;
	std::string outputs;
	std::map<std::string, double> inputs;
};
} // namespace mappers

#endif // FACTORY_TYPE_H