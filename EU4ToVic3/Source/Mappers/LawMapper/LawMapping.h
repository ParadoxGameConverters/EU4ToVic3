#ifndef LAW_MAPPING_H
#define LAW_MAPPING_H
#include "Parser.h"
#include <set>

namespace mappers
{
class LawMapping: commonItems::parser
{
  public:
	LawMapping() = default;
	explicit LawMapping(std::istream& theStream);

	[[nodiscard]] const auto& getForms() const { return forms; }
	[[nodiscard]] const auto& getIdeas() const { return ideas; }
	[[nodiscard]] const auto& getReforms() const { return reforms; }

	[[nodiscard]] double getScore(const std::string& incForm, const std::set<std::string>& incIdeas, const std::set<std::string>& incReforms) const;

  private:
	void registerKeys();

	std::set<std::string> forms;
	std::set<std::string> ideas;
	std::set<std::string> reforms;
};
} // namespace mappers

#endif // LAW_MAPPING_H