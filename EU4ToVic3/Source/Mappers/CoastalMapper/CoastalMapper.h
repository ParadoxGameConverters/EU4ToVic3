#ifndef COASTAL_MAPPER_H
#define COASTAL_MAPPER_H
#include "Parser.h"

namespace mappers
{
class CoastalMapper: commonItems::parser
{
  public:
	CoastalMapper() = default;
	void loadAdjacencies(const std::string& filePath);

	[[nodiscard]] std::set<std::string> getAdjacencies(const std::string& province) const;

  private:
	void registerKeys();

	std::map<std::string, std::set<std::string>> adjacencies;
};
} // namespace mappers

#endif // COASTAL_MAPPER_H