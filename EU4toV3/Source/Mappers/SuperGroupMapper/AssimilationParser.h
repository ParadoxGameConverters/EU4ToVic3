#ifndef ASSIMILATION_PARSER
#define ASSIMILATION_PARSER
#include "ConvenientParser.h"

namespace mappers
{
class AssimilationParser: commonItems::convenientParser
{
  public:
	explicit AssimilationParser(std::istream& theStream);

	[[nodiscard]] auto getAssimilation() const { return assimilation; }

  private:
	int assimilation = 1;
};
} // namespace mappers

#endif // ASSIMILATION_PARSER