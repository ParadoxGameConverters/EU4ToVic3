#ifndef V3_CHUNK_H
#define V3_CHUNK_H
#include "Province.h"
#include "SourceProvinceData.h"
#include <map>
#include <memory>
#include <optional>

/* A Chunk is a collection of provinces defined by province_mappings.txt where a particular eu4 province maps to.
 * Its defining attribute is the single owner these provinces belong to. From Chunks we can generate substates, when
 * we overlay this political map over a geographical map.
 *
 * A chunk can be as small as a single V3 province, or as large as multiple V3 states.*/

namespace EU4
{
class Country;
class PopRatio;
class Province;
} // namespace EU4
namespace V3
{
class Country;
class SubState;
class Chunk
{
  public:
	Chunk() = default;

	void addProvince(const std::pair<std::string, std::shared_ptr<Province>>& province) { provinces.emplace(province); }
	void setSourceOwnerTag(const std::string& sourceTag) { sourceOwnerTag = sourceTag; }
	void setCapital() { capital = true; }

	[[nodiscard]] const auto& getProvinces() const { return provinces; }
	[[nodiscard]] const auto& getSourceOwnerTag() const { return sourceOwnerTag; }
	[[nodiscard]] bool isSea() const;
	[[nodiscard]] const auto& getSourceProvinceData() const { return weightedSourceProvinceData; }
	[[nodiscard]] double getTotalSourceProvinceWeight() const;
	[[nodiscard]] auto isCapital() const { return capital; }

	void addSourceProvinceData(const EU4::Province& sourceProvince, double weight);
	[[nodiscard]] std::map<std::string, double> calcOwnerWeights() const;
	[[nodiscard]] std::set<std::string> getKnownCapitals() const;

  private:
	std::map<std::string, std::shared_ptr<Province>> provinces; // V3 provinces
	std::optional<std::string> sourceOwnerTag;
	std::vector<std::pair<SourceProvinceData, double>> weightedSourceProvinceData;
	bool capital = false;
};
} // namespace V3

#endif // V3_CHUNK_H
