#include "ClayManager.h"
#include "Log.h"
#include "StateLoader/StateLoader.h"
#include <ranges>

void V3::ClayManager::initializeVanillaStates(const std::string& v3Path)
{
	StateLoader stateLoader;
	stateLoader.loadStates(v3Path);
	states = stateLoader.getStates();

	auto provinceCount = 0;
	for (const auto& state: states | std::views::values)
		provinceCount += static_cast<int>(state->getProvinces().size());
	Log(LogLevel::Info) << "> " << states.size() << " states loaded with " << provinceCount << " provinces inside.";
}
