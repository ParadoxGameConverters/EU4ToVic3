#ifndef OUT_DIPLOMATIC_PLAYS_H
#define OUT_DIPLOMATIC_PLAYS_H
#include "PoliticalManager/PoliticalManager.h"

namespace OUT
{
void exportDiplomaticPlays(const std::string& outputName, const std::map<std::string, std::shared_ptr<V3::Country>>& countries);

} // namespace OUT

#endif // OUT_DIPLOMATIC_PLAYS_H