#include "State.h"
#include "../Province/Province.h"

V2::State::State(const int newId, std::shared_ptr<Province> firstProvince): id(newId)
{
	provinces.push_back(firstProvince);
}

void V2::State::setProvincesAsTerritories()
{
	for (const auto& province: provinces)
	{
		 // We are NOT affecting colonies in progress!
		if (!province->getColonyLevel()) 
			province->setColonial(2);
	}
}

void V2::State::setProvincesAsStates()
{
	for (const auto& province: provinces)
	{
		// We are affecting colonies in progress! (This is used for uncivilized nations that may not own territories)
		province->setColonial(0);
	}
}

void V2::State::addRailroads()
{
	for (const auto& province: provinces)
		province->increaseRailLevel(1);
}

bool V2::State::isCoastal() const
{
	for (const auto& province: provinces)
		if (province->isCoastal())
			return true;
	return false;
}

bool V2::State::hasLocalSupply(const std::string& product) const
{
	for (const auto& province: provinces)
		if (province->getRgoType() == product)
			return true;
	return false;
}

double V2::State::getSuppliedInputs(std::shared_ptr<Factory> factory) const
{
	// find out the needs
	std::map<std::string, double> inputs = factory->getInputs();
	const int numNeeds = static_cast<int>(inputs.size());
	if (!numNeeds)
		return 0;

	// find out what we have from both RGOs and existing factories
	std::map<std::string, double> supplies;
	for (const auto& province: provinces)
		supplies[province->getRgoType()] += 1.0;
	for (const auto& factoryItr: factories)
		supplies[factoryItr->getOutputs()] += 1.0;

	// determine how many of the inputs are supplied
	int totalSupplied = 0;
	for (const auto& inputItr: inputs)
	{
		const auto& supplyItr = supplies.find(inputItr.first);
		if (supplyItr != supplies.end())
			totalSupplied++;
	}

	return static_cast<double>(totalSupplied) / numNeeds;
}

bool V2::State::provInState(const int id) const
{
	for (const auto& province: provinces)
		if (province->getID() == id)
			return true;
	return false;
}

void V2::State::addFactory(std::shared_ptr<Factory> factory)
{
	auto addFactory = provinces[0]->addFactory(std::move(factory));
	// Only add factory to the register if it's added to province register.
	if (addFactory)
		factories.push_back(*addFactory);
}

double V2::State::getMfgRatio() const
{
	// count the manufactories in the source provinces
	double numManus = 0;
	for (const auto& province: provinces)
		numManus += province->getMfgCount();
	return numManus / provinces.size();
}

void V2::State::rebuildNavalBase()
{
	// Only one naval base in a state
	std::shared_ptr<Province> prov = nullptr;
	auto level = 0;
	for (const auto& province: provinces)
	{
		if (!prov)
			prov = province;
		if (level < province->getNavalBaseLevel())
		{
			prov = province;
			level = province->getNavalBaseLevel();
		}
	}
	for (const auto& province: provinces)
		province->setNavalBaseLevel(0);
	if (prov)
		prov->setNavalBaseLevel(level);
}