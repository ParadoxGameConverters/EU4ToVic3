#ifndef PROVINCE_MANAGER
#define PROVINCE_MANAGER
#include "EU4Province.h"
#include "Parser.h"
#include <map>

namespace EU4
{
class ProvinceManager: commonItems::parser
{
  public:
	ProvinceManager() = default;
	explicit ProvinceManager(std::istream& theStream);

	[[nodiscard]] const auto& getAllProvinces() const { return provinces; }
	[[nodiscard]] const std::shared_ptr<Province>& getProvince(int provinceNumber) const;

  private:
	void registerKeys();

	std::map<int, std::shared_ptr<Province>> provinces;
};
} // namespace EU4

#endif // PROVINCE_MANAGER