#ifndef OUT_BUILDINGS_H
#define OUT_BUILDINGS_H
#include "ClayManager/State/State.h"

namespace OUT
{
void exportBuildings(const std::string& outputName, const std::map<std::string, std::shared_ptr<V3::State>>& states);
} // namespace OUT

#endif // OUT_BUILDINGS_H