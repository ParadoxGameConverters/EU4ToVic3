#ifndef EU4_MOD_H
#define EU4_MOD_H
#include "ConvenientParser.h"

namespace EU4
{
class Mod: commonItems::convenientParser
{
  public:
	explicit Mod(std::istream& theStream);

	[[nodiscard]] const auto& getName() const { return name; }
	[[nodiscard]] const auto& getPath() const { return path; }
	[[nodiscard]] auto isValid() const { return !name.empty() && !path.empty(); }
	[[nodiscard]] auto isCompressed() const { return compressed; }

	void setPath(const std::string& thePath) { path = thePath; }

  private:
	void registerKeys();

	std::string name;
	std::string path;
	bool compressed = false;
};
} // namespace EU4

#endif // EU4_MOD_H