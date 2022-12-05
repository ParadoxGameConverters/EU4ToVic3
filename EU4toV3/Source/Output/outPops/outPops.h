#ifndef OUT_POPS_H
#define OUT_POPS_H
#include "ClayManager/State/State.h"

namespace OUT
{
void exportPops(const std::string& outputName, const std::map<std::string, std::shared_ptr<V3::State>>& states);
void outPop(std::ostream& output, const V3::Pop& pop);
void outStatePops(std::ostream& output, const V3::State& state);
void outSubStatePops(std::ostream& output, const V3::SubState& subState);

} // namespace OUT

#endif // OUT_POPS_H