#ifndef VN_COLONIAL_MAPPING
#define VN_COLONIAL_MAPPING
#include "Parser.h"

namespace mappers
{
class VNColonialMapping: commonItems::parser
{
  public:
	explicit VNColonialMapping(std::istream& theStream);

	[[nodiscard]] const auto& getName() const { return name; }
	[[nodiscard]] const auto& getOwner() const { return owner; }
	[[nodiscard]] const auto& getDecolonizeBlocker() const { return decolonizeBlocker; }
	[[nodiscard]] const auto& getKeyProvince() const { return keyProvince; }
	[[nodiscard]] const auto& getStates() const { return states; }
	[[nodiscard]] const auto& getSubjects() const { return subjects; }
	[[nodiscard]] const auto& getSubjectType() const { return subjectType; }

  private:
	void registerKeys();

	std::string name;
	std::string owner;
	std::string decolonizeBlocker;
	std::string keyProvince;
	std::set<std::string> states;
	std::set<std::string> subjects;
	std::string subjectType;
};
} // namespace mappers

#endif // VN_COLONIAL_MAPPING