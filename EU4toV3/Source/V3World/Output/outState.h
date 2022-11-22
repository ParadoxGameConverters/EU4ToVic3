#ifndef OUT_STATE_H
#define OUT_STATE_H
#include "ClayManager/StateLoader/State.h"

namespace V3
{
std::ostream& operator<<(std::ostream& output, const State& state);

} // namespace V3

#endif // OUT_STATE_H