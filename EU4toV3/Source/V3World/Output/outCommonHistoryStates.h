#ifndef OUT_COMMON_HISTORY_STATES_H
#define OUT_COMMON_HISTORY_STATES_H
#include "ClayManager/StateLoader/State.h"

namespace V3
{
std::ostream& outCommonHistoryStates(std::ostream& output, const std::map<std::string, std::shared_ptr<State>>& states);

} // namespace V3

#endif // OUT_COMMON_HISTORY_STATES_H