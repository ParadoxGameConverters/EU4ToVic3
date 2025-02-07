#ifndef V3_BUILDING_RESOURCES_H
#define V3_BUILDING_RESOURCES_H
#include "ProductionMethods/ProductionMethod.h"
#include "ProductionMethods/ProductionMethodGroup.h"

namespace V3
{
class BuildingResources
{
  public:
	BuildingResources() = default;

	void addJobs(const std::string& job, const double amount) { jobs[job] += std::max(0.0, amount); }
	void addInputs(const std::string& input, const double amount) { inputs[input] += std::max(0.0, amount); }
	void addOutputs(const std::string& output, const double amount) { outputs[output] += std::max(0.0, amount); }
	void evaluateResources(const std::set<std::string>& usedPMGroups,
		 const std::map<std::string, std::tuple<int, double>>& estimatedPMs,
		 const std::map<std::string, ProductionMethod>& PMs,
		 const std::map<std::string, ProductionMethodGroup>& PMGroups);

	[[nodiscard]] const auto& getJobs() const { return jobs; }
	[[nodiscard]] const auto& getInputs() const { return inputs; }
	[[nodiscard]] const auto& getOutputs() const { return outputs; }


  private:
	std::map<std::string, double> jobs;
	std::map<std::string, double> inputs;
	std::map<std::string, double> outputs;
};
} // namespace V3

#endif // V3_BUILDING_RESOURCES_H