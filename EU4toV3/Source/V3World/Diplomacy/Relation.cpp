#include "Relation.h"

V2::Relation::Relation(std::string _target):
	target(std::move(_target)){}

V2::Relation::Relation(std::string _target, EU4::EU4RelationDetails& oldRelations):
	 target(std::move(_target)), relations(oldRelations.getRelations()), militaryAccess(oldRelations.hasMilitaryAccess()){}

void V2::Relation::setLevel(int lvl)
{
	if (lvl < 0 || lvl > 5) return;
	level = lvl;
}
