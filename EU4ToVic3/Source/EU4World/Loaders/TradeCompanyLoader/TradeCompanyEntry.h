#ifndef TRADE_COMPANY_ENTRY
#define TRADE_COMPANY_ENTRY
#include "Parser.h"
#include "TradeCompany.h"

namespace EU4
{
class TradeCompanyEntry: commonItems::parser
{
  public:
	TradeCompanyEntry() = default;
	explicit TradeCompanyEntry(std::istream& theStream);

	[[nodiscard]] const auto& getTradeCompany() const { return tradeCompany; }

  private:
	void registerKeys();

	TradeCompany tradeCompany;
};
} // namespace EU4

#endif // TRADE_COMPANY_ENTRY
