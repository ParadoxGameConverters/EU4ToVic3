#ifndef CULTURE_PARSER
#define CULTURE_PARSER
#include "Parser.h"

namespace EU4
{
class CultureParser: commonItems::parser
{
  public:
	CultureParser() = default;
	explicit CultureParser(std::istream& theStream);

	[[nodiscard]] const auto& getPrimaryTag() const { return primaryTag; }
	[[nodiscard]] const auto& getMaleNames() const { return maleNames; }
	[[nodiscard]] const auto& getFemaleNames() const { return femaleNames; }
	[[nodiscard]] const auto& getDynastyNames() const { return dynastyNames; }

	void setPrimaryTag(const std::string& theTag) { primaryTag = theTag; }
	void addMaleNames(const std::vector<std::string>& theNames) { maleNames.insert(maleNames.end(), theNames.begin(), theNames.end()); }
	void addFemaleNames(const std::vector<std::string>& theNames) { femaleNames.insert(femaleNames.end(), theNames.begin(), theNames.end()); }
	void addDynastyNames(const std::vector<std::string>& theNames) { dynastyNames.insert(dynastyNames.end(), theNames.begin(), theNames.end()); }

  private:
	void registerKeys();

	std::string primaryTag;
	std::vector<std::string> maleNames;
	std::vector<std::string> femaleNames;
	std::vector<std::string> dynastyNames;
};
} // namespace EU4

#endif // CULTURE_PARSER
