#ifndef STATE_H
#define STATE_H

#include <memory>
#include <string>
#include <vector>

namespace V2
{
class Factory;
class Province;
class State
{
  public:
	State(int newId, std::shared_ptr<Province> firstProvince);

	void setColonial(const bool col) { colonial = col; }
	void addProvince(std::shared_ptr<Province> newProvince) { provinces.push_back(newProvince); }

	[[nodiscard]] auto isColonial() const { return colonial; }
	[[nodiscard]] auto getFactoryCount() const { return factories.size(); }
	[[nodiscard]] auto getID() const { return id; }
	[[nodiscard]] const auto& getProvinces() const { return provinces; }

	[[nodiscard]] bool isCoastal() const;
	[[nodiscard]] bool hasLocalSupply(const std::string& product) const;
	[[nodiscard]] double getSuppliedInputs(std::shared_ptr<Factory> factory) const;
	[[nodiscard]] bool provInState(int id) const;
	[[nodiscard]] double getMfgRatio() const;

	void addRailroads();
	void addFactory(std::shared_ptr<Factory> factory);
	void rebuildNavalBase();
	void setProvincesAsTerritories();
	void setProvincesAsStates();

  private:
	int id = 0;
	bool colonial = false;
	std::vector<std::shared_ptr<Province>> provinces;
	std::vector<std::shared_ptr<Factory>> factories;
};
} // namespace V2

#endif // STATE_H