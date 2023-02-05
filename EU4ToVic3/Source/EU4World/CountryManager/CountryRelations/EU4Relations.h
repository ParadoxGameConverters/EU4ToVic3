#ifndef EU4RELATIONS_H
#define EU4RELATIONS_H
#include "EU4RelationDetails.h"
#include "Parser.h"
#include <map>

namespace EU4
{
class EU4Relations: commonItems::parser
{
  public:
	explicit EU4Relations(std::istream& theStream);

	[[nodiscard]] const auto& getRelations() const { return relations; }

  private:
	void registerKeys();

	std::map<std::string, EU4RelationDetails> relations;
};
} // namespace EU4

#endif // EU4RELATIONS_H