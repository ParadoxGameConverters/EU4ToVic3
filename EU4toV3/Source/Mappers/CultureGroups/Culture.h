#ifndef CULTURE_H
#define CULTURE_H
#include "Color.h"
#include "Parser.h"

namespace mappers
{
class Culture: commonItems::parser
{
  public:
	Culture() = default;
	explicit Culture(std::istream& theStream);

	[[nodiscard]] const auto& getHpm() const { return hpm; }
	[[nodiscard]] const auto& getMaleNames() const { return maleNames; }
	[[nodiscard]] const auto& getFemaleNames() const { return femaleNames; }
	[[nodiscard]] const auto& getFirstNames() const { return firstNames; }
	[[nodiscard]] const auto& getLastNames() const { return lastNames; }
	[[nodiscard]] const auto& getDynastyNames() const { return dynastyNames; }
	[[nodiscard]] const auto& getOriginalCulture() const { return originalCulture; }
	[[nodiscard]] const auto& getColor() const { return color; }
	[[nodiscard]] const auto& getPrimaryTag() const { return primaryTag; }
	[[nodiscard]] auto isNeoCulture() const { return neoCulture; }
	[[nodiscard]] auto getRadicalism() const { return radicalism; }

	void setFirstNames(const std::vector<std::string>& theFirstNames) { firstNames = theFirstNames; }
	void setLastNames(const std::vector<std::string>& theLastNames) { lastNames = theLastNames; }
	void setColor(const commonItems::Color& theColor) { color = theColor; }

	void addMaleNames(const std::vector<std::string>& names) { maleNames.insert(maleNames.end(), names.begin(), names.end()); }
	void addFemaleNames(const std::vector<std::string>& names) { femaleNames.insert(femaleNames.end(), names.begin(), names.end()); }
	void addDynastyNames(const std::vector<std::string>& names) { dynastyNames.insert(dynastyNames.end(), names.begin(), names.end()); }
	void setNeoCulture(const bool nc)
	{
		neoCulture = nc;
		radicalism = 10;
	}
	void setOriginalCulture(const std::string& origName) { originalCulture = origName; }

	void transmogrify();

	friend std::ostream& operator<<(std::ostream& output, const Culture& culture);

  private:
	void registerKeys();

	std::string hpm;
	std::string primaryTag;
	std::vector<std::string> maleNames;
	std::vector<std::string> femaleNames;
	std::vector<std::string> firstNames;
	std::vector<std::string> lastNames;
	std::vector<std::string> hpmLastNames;
	std::vector<std::string> dynastyNames;
	std::optional<commonItems::Color> color;
	int radicalism = 0;
	bool neoCulture = false;	  // culture generated on-the-fly by the converter
	std::string originalCulture; // relevant only for neocultures so we know where they originated from.
};
} // namespace mappers

#endif // CULTURE_H
