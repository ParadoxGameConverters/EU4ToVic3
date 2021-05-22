#ifndef PROVINCES_H
#define PROVINCES_H
#include "EU4Province.h"
#include "Parser.h"
#include <map>

namespace mappers
{
class SuperGroupMapper;
}

namespace EU4
{
class Regions;
class Provinces: commonItems::parser
{
  public:
	explicit Provinces(std::istream& theStream);

	[[nodiscard]] const auto& getAllProvinces() const { return provinces; }
	[[nodiscard]] const std::shared_ptr<Province>& getProvince(int provinceNumber) const;

  private:
	void registerKeys();

	std::map<int, std::shared_ptr<Province>> provinces;
};
} // namespace EU4

#endif // PROVINCES_H