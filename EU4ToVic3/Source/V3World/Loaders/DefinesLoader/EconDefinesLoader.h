#ifndef ECON_DEFINES_LOADER_H
#define ECON_DEFINES_LOADER_H
#include "Parser.h"

namespace V3
{
class EconDefinesLoader: commonItems::parser
{
  public:
	EconDefinesLoader() = default;

	void loadEconDefines(const std::string& filePath);
	[[nodiscard]] const auto& getGlobalCP() const { return globalCP; }
	[[nodiscard]] const auto& getVNGlobalCP() const { return VNGlobalCP; }
	[[nodiscard]] const auto& getCentralizedPopRatio() const { return centralizedPopRatio; }
	[[nodiscard]] const auto& getMeanCivlevel() const { return meanCivlevel; }
	[[nodiscard]] const auto& getStateTraitStrength() const { return stateTraitStrength; }
	[[nodiscard]] const auto& getPacketFactor() const { return packetFactor; }
	[[nodiscard]] const auto& getIncorporatedModifier() const { return incorpModifier; }
	[[nodiscard]] const auto& getMinDevPerPop() const { return minDevPerPop; }
	[[nodiscard]] const auto& getMaxDevPerPop() const { return maxDevPerPop; }

  private:
	void registerKeys();

	int globalCP = 0;
	int VNGlobalCP = 0;
	double centralizedPopRatio = 0;
	double meanCivlevel = 0;
	double stateTraitStrength = 0;
	double packetFactor = 0;
	double incorpModifier = 0.0;
	double minDevPerPop = 0.0;
	double maxDevPerPop = 1.0;
};
} // namespace V3
#endif // ECON_DEFINES_LOADER_H