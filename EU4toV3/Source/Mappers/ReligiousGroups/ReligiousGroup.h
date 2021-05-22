#ifndef RELIGIOUS_GROUP_H
#define RELIGIOUS_GROUP_H
#include "Parser.h"
#include <map>

namespace mappers
{
class Religion;
class ReligiousGroup: commonItems::parser
{
  public:
	ReligiousGroup() = default;
	ReligiousGroup(std::string theName, std::istream& theStream);
	
	[[nodiscard]] const auto& getHpm() const { return hpm; }

	friend std::ostream& operator<<(std::ostream& output, const ReligiousGroup& religiousGroup);

  private:
	void registerKeys();
	
	std::string name;
	std::string hpm;
	std::map<std::string, std::shared_ptr<Religion>> religions;
};
} // namespace mappers

#endif // RELIGIOUS_GROUP_H
