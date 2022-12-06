#ifndef OUT_POPS_H
#define OUT_POPS_H
#include "ClayManager/State/State.h"

namespace OUT
{
void exportPops(const std::string& outputName, const std::map<std::string, std::shared_ptr<V3::State>>& states);

} // namespace OUT

#endif // OUT_POPS_H