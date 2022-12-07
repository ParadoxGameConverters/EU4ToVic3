#ifndef RELIGION_ENTRY
#define RELIGION_ENTRY
#include "Color.h"
#include "Parser.h"

namespace EU4
{
class ReligionEntry: commonItems::parser
{
  public:
	ReligionEntry(std::istream& theStream);
	[[nodiscard]] const auto& getColor() const { return color; }

  private:
	void registerKeys();

	std::optional<commonItems::Color> color;
};
} // namespace EU4

#endif // RELIGION_ENTRY