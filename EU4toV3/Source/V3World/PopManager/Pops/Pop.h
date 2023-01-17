#ifndef POP_H
#define POP_H
#include <string>

namespace V3
{
class Pop
{
  public:
	Pop() = default;
	Pop(std::string culture, std::string religion, std::string type, int size):
		 culture(std::move(culture)), religion(std::move(religion)), type(std::move(type)), size(size)
	{
	}
	bool operator==(const Pop& rhs) const { return culture == rhs.culture && religion == rhs.religion && type == rhs.type; }

	[[nodiscard]] const auto& getCulture() const { return culture; }
	[[nodiscard]] const auto& getReligion() const { return religion; }
	[[nodiscard]] const auto& getType() const { return type; }
	[[nodiscard]] auto getSize() const { return size; }
	[[nodiscard]] auto isMinority() const { return minority; }

	void setCulture(const std::string& theCulture) { culture = theCulture; }
	void setReligion(const std::string& theReligion) { religion = theReligion; }
	void setType(const std::string& theType) { type = theType; }
	void setSize(int theSize) { size = theSize; }
	void blankReligion() { religion.clear(); }
	void blankCulture() { culture.clear(); }
	void setMinority() { minority = true; }

  private:
	std::string culture;
	std::string religion;
	std::string type;
	int size = 0;
	bool minority = false; // Minorities tend to persist unaltered.
};
} // namespace V3
#endif // POP_H