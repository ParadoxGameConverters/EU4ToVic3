#ifndef POP_TYPES_H
#define POP_TYPES_H
#include "Parser.h"
#include "PopInstance.h"

namespace mappers
{
class PopTypes: commonItems::parser
{
  public:
	PopTypes() = default;
	explicit PopTypes(std::istream& theStream);

	[[nodiscard]] const auto& getPopTypes() const { return popTypes; }

  private:
	void registerKeys();

	std::vector<PopDetails> popTypes;
};
} // namespace mappers

#endif // POP_TYPES_H
