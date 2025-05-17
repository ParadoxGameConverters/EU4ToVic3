#ifndef OUT_CHARACTERS_H
#define OUT_CHARACTERS_H
#include "PoliticalManager/PoliticalManager.h"

namespace OUT
{
void exportCharacters(const std::filesystem::path& outputName, const std::map<std::string, std::shared_ptr<V3::Country>>& countries);

} // namespace OUT

#endif // OUT_CHARACTERS_H