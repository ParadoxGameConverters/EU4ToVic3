#ifndef EU4_MOD_H
#define EU4_MOD_H
#include "ConvenientParser.h"

namespace EU4
{
class Mod: commonItems::convenientParser
{
  public:
	explicit Mod(std::istream& theStream);
	void overLoad(const std::string& filePath);

	[[nodiscard]] const auto& getName() const { return name; }
	[[nodiscard]] const auto& getPath() const { return path; }
	[[nodiscard]] auto isValid() const { return !name.empty() && !path.empty(); }
	[[nodiscard]] auto isCompressed() const { return compressed; }

  private:
	void registerKeys();

	std::string name;
	std::string path;
	bool compressed = false;
};
} // namespace EU4

#endif // EU4_MOD_H