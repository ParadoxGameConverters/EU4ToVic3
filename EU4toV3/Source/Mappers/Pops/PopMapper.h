#ifndef POP_MAPPER_H
#define POP_MAPPER_H
#include "Parser.h"
#include "PopTypes.h"

namespace mappers
{
class PopMapper: commonItems::parser
{
  public:
	PopMapper() = default;
	explicit PopMapper(const std::string& fileName);
	explicit PopMapper(std::istream& theStream);

	[[nodiscard]] const auto& getProvincePops() const { return provincePops; }

  private:
	void registerKeys();
	std::map<int, std::vector<PopDetails>> provincePops;
};
} // namespace mappers

#endif // POP_MAPPER_H
