#ifndef NAVAL_BASE_H
#define NAVAL_BASE_H
#include "Parser.h"

namespace mappers
{
class NavalBase: commonItems::parser
{
  public:
	NavalBase() = default;
	explicit NavalBase(std::istream& theStream);

	[[nodiscard]] auto isFound() const { return found; }

  private:
	void registerKeys();

	bool found = false;
};
} // namespace mappers

#endif // NAVAL_BASE_H
