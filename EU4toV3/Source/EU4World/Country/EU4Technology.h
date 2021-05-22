#ifndef EU4_TECHNOLOGY_H
#define EU4_TECHNOLOGY_H
#include "Parser.h"

namespace EU4
{
class EU4Technology: commonItems::parser
{
  public:
	EU4Technology() = default;
	explicit EU4Technology(std::istream& theStream);

	[[nodiscard]] auto getAdm() const { return adm; }
	[[nodiscard]] auto getDip() const { return dip; }
	[[nodiscard]] auto getMil() const { return mil; }

  private:
	void registerKeys();

	int adm = 0;
	int dip = 0;
	int mil = 0;
};
} // namespace EU4

#endif // EU4_TECHNOLOGY_H