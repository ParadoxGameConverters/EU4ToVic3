#ifndef PORT_PROVINCES_H
#define PORT_PROVINCES_H
#include "Parser.h"
#include <set>

namespace mappers
{
// hack for naval bases.  not ALL naval bases are in port provinces, and if a navy spawns at a naval base in
// a non-port province, Vicky crashes....
class PortProvinces: commonItems::parser
{
  public:
	PortProvinces();
	explicit PortProvinces(std::istream& theStream);

	[[nodiscard]] auto isProvinceIDDenied(const int& provinceID) const { return denylist.contains(provinceID); }
	[[nodiscard]] auto isProvinceIDAllowed(const int& provinceID) const { return allowlist.contains(provinceID); }

  private:
	void registerKeys();

	std::set<int> allowlist;
	std::set<int> denylist;
};
} // namespace mappers

#endif // PORT_PROVINCES_H
