#ifndef LAW_ENTRY_H
#define LAW_ENTRY_H
#include "Law.h"
#include "Parser.h"

namespace V3
{
class LawEntry: commonItems::parser
{

  public:
	LawEntry() = default;
	explicit LawEntry(std::istream& theStream);

	[[nodiscard]] const auto& getLaw() const { return law; }

  private:
	void registerKeys();

	Law law;
};
} // namespace V3
#endif // LAW_ENTRY_H