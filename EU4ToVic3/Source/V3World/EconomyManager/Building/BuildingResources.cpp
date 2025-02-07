#include "BuildingResources.h"

void V3::BuildingResources::evaluateResources(const std::set<std::string>& usedPMGroups,
	 const std::map<std::string, std::tuple<int, double>>& estimatedPMs,
	 const std::map<std::string, ProductionMethod>& PMs,
	 const std::map<std::string, ProductionMethodGroup>& PMGroups)
{
	for (const auto& PMGroupName: usedPMGroups)
	{
		int index = 0;
		double fraction = 1.0;

		// If a rule exists
		if (const auto& estimateIter = estimatedPMs.find(PMGroupName); estimateIter != estimatedPMs.end())
		{
			const auto& [i, p] = estimateIter->second;
			index = i;
			fraction = p;
		}

		// PMs and PMGroups previously validated by the PM estimation process
		const auto& basePM = PMs.at(PMGroups.at(PMGroupName).getPMs()[0]);
		const auto& advancedPM = PMs.at(PMGroups.at(PMGroupName).getPMs()[index]);

		if (fraction < 1)
		{
			for (const auto& [job, amount]: basePM.getEmployment())
			{
				jobs[job] += amount * (1 - fraction);
			}
			for (const auto& [input, amount]: basePM.getInputs())
			{
				inputs[input] += amount * (1 - fraction);
			}
			for (const auto& [output, amount]: basePM.getOutputs())
			{
				outputs[output] += amount * (1 - fraction);
			}
		}

		for (const auto& [job, amount]: advancedPM.getEmployment())
		{
			jobs[job] += amount * fraction;
		}
		for (const auto& [input, amount]: advancedPM.getInputs())
		{
			inputs[input] += amount * fraction;
		}
		for (const auto& [output, amount]: advancedPM.getOutputs())
		{
			outputs[output] += amount * fraction;
		}
	}
}