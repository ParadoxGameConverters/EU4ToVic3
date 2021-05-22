#ifndef FACTORY_H
#define FACTORY_H

#include <ostream>
#include "../../Mappers/FactoryTypes/FactoryType.h"

namespace V2
{
	class Factory
	{
	public:
		Factory() = default;
		explicit Factory(const mappers::FactoryType& fType) { factoryType = fType; }
		
		void increaseLevel() { level++; }

		[[nodiscard]] auto requiresCoastal() const { return factoryType.isCoastal(); }
		[[nodiscard]] const auto& getRequiredTech() const { return factoryType.getRequiredTech(); }
		[[nodiscard]] const auto& getRequiredInvention() const { return factoryType.getRequiredInvention(); }
		[[nodiscard]] const auto& getTypeName() const { return factoryType.getName(); }
		[[nodiscard]] const auto& getInputs() const { return factoryType.getInputs(); }
		[[nodiscard]] const auto& getOutputs() const { return factoryType.getOutputs(); };

		friend std::ostream& operator<<(std::ostream& output, const Factory& factory);

	private:
		mappers::FactoryType factoryType;
		int level = 1;
	};
}

#endif // FACTORY_H
