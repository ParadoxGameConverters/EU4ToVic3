#ifndef EU4_MODNAMES_H
#define EU4_MODNAMES_H
#include "ConvenientParser.h"

namespace EU4
{
class ModNames: commonItems::convenientParser
{
  public:
	explicit ModNames(std::istream& theStream);

	[[nodiscard]] const auto& getName() const { return name; }
	[[nodiscard]] const auto& getPath() const { return path; }

  private:
	void registerKeys();

	std::string name;
	std::string path;
};
} // namespace EU4

#endif // EU4_MODNAMES_H