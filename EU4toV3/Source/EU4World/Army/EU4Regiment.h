#ifndef EU4_REGIMENT_H
#define EU4_REGIMENT_H
#include "Parser.h"

namespace EU4
{
class EU4Regiment: public commonItems::parser
{
  public:
	EU4Regiment() = default;
	explicit EU4Regiment(std::istream& theStream); // Also applies to ships

	[[nodiscard]] const auto& getType() const { return regimentType; }
	[[nodiscard]] const auto& getName() const { return name; }
	[[nodiscard]] auto getHome() const { return home; }
	[[nodiscard]] auto getCategory() const { return category; }
	[[nodiscard]] auto getTypeStrength() const { return typeStrength; }

	void setCategory(const std::string& cat) { category = cat; }
	void setTypeStrength(const int tStrength) { typeStrength = tStrength; }

  private:
	void registerKeys();

	std::string name;
	std::string regimentType;
	int home = 0;
	int typeStrength = 0;
	std::string category;
};
} // namespace EU4

#endif // EU4_REGIMENT_H