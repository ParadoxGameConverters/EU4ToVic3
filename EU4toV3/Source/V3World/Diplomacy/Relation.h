#ifndef RELATION_H
#define RELATION_H

#include "Date.h"
#include "../../EU4World/Relations/EU4Relations.h"

namespace V2
{
	class Relation
	{
	public:
		Relation(std::string _target);
		Relation(std::string _target, EU4::EU4RelationDetails& oldRelations);

		void setLevel(int lvl);
		void setRelations(int value) { relations = value; }
		void increaseRelations(int value) { relations += value; }
		void setInfluence(int value) { influence = value; }
		void setAccess(bool access) { militaryAccess = access; }
		
		[[nodiscard]] const auto& getTarget() const { return target; }
		[[nodiscard]] auto getRelations() const { return relations; }
		[[nodiscard]] auto getInfluence() const { return influence; }
		[[nodiscard]] auto getLevel() const { return level; }

		friend std::ostream& operator<<(std::ostream& output, const Relation& relation);

	private:
		std::string target;
		int relations = 0;
		int influence = 0;
		bool militaryAccess = false;
		int level = 2;
	};
}

#endif // RELATIONS_H