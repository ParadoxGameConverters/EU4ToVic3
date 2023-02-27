#ifndef OUT_COMMON_HISTORY_STATES_H
#define OUT_COMMON_HISTORY_STATES_H
#include "ClayManager/State/State.h"

namespace OUT
{
void exportCommonHistoryStates(const std::string& outputName, const std::map<std::string, std::shared_ptr<V3::State>>& states);

} // namespace OUT

#endif // OUT_COMMON_HISTORY_STATES_H