#ifndef POP_H
#define POP_H
#include <string>
#include <vector>

namespace V3
{
class Pop
{
  public:
	Pop() = default;
	Pop(std::string culture, std::string religion, std::string type, int size);
	bool operator==(const Pop& rhs) const;

	[[nodiscard]] const auto& getCulture() const { return culture; }
	[[nodiscard]] const auto& getReligion() const { return religion; }
	[[nodiscard]] const auto& getType() const { return type; }
	[[nodiscard]] auto getSize() const { return size; }
	[[nodiscard]] auto isMinority() const { return minority; }

	void setCulture(const std::string& theCulture) { culture = theCulture; }
	void setReligion(const std::string& theReligion) { religion = theReligion; }
	void setType(const std::string& theType) { type = theType; }
	void setSize(int theSize) { size = theSize; }

  private:
	std::string culture;
	std::string religion;
	std::string type;
	int size = 0;
	bool minority = false; // Minorities tend to persist unaltered.
};

class SubStatePops
{
  public:
	SubStatePops() = default;

	void setPops(const std::vector<Pop>& thePops) { pops = thePops; }
	void setTag(const std::string& theTag) { tag = theTag; }

	[[nodiscard]] int getPopCount() const;
	[[nodiscard]] const auto& getPops() const { return pops; }
	[[nodiscard]] const auto& getTag() const { return tag; }

  private:
	std::string tag;
	std::vector<Pop> pops;
};

class StatePops
{
  public:
	StatePops() = default;

	void setSubStatePops(const std::vector<SubStatePops>& thePops) { subStatePops = thePops; }
	void setStateName(const std::string& theName) { stateName = theName; }

	[[nodiscard]] int getPopCount() const;
	[[nodiscard]] const auto& getSubStatePops() const { return subStatePops; }
	[[nodiscard]] const auto& getStateName() const { return stateName; }

  private:
	std::string stateName;
	std::vector<SubStatePops> subStatePops;
};
} // namespace V3
#endif // POP_H