#ifndef LAW_GROUP_H
#define LAW_GROUP_H
#include "LawMapping.h"
#include "Parser.h"

namespace mappers
{
class LawGroup: commonItems::parser
{
  public:
	LawGroup() = default;
	explicit LawGroup(std::istream& theStream);

	[[nodiscard]] const auto& getLaws() const { return laws; }

  private:
	void registerKeys();

	std::map<std::string, LawMapping> laws;
};
} // namespace mappers

#endif // LAW_GROUP_H