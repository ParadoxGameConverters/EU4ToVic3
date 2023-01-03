#ifndef RELATION_H
#define RELATION_H
#include "CountryManager/CountryRelations/EU4Relations.h"

namespace V3
{
class Relation
{
  public:
	Relation(std::string _target): target(std::move(_target)) {}
	Relation(std::string _target, const EU4::EU4RelationDetails& oldRelations): target(std::move(_target)), relations(oldRelations.getRelations()) {}

	void setLevel(int lvl);
	void setRelations(int value) { relations = value; }
	void increaseRelations(int value) { relations += value; }

	[[nodiscard]] const auto& getTarget() const { return target; }
	[[nodiscard]] auto getRelations() const { return relations; }

  private:
	std::string target;
	int relations = 0;
};
} // namespace V3

#endif // RELATIONS_H