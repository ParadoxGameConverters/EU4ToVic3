#ifndef OUTPUT_H
#define OUTPUT_H

#include "../../Mappers/VersionParser/VersionParser.h"
#include "../Army/Army.h"
#include "../Army/Regiment.h"
#include "../Country/Country.h"
#include "../Diplomacy/Agreement.h"
#include "../Diplomacy/Relation.h"
#include "../Factory/Factory.h"
#include "../Leader/Leader.h"
#include "../Localisation/Localisation.h"
#include "../Party/Party.h"
#include "../Pop/Pop.h"
#include "../Province/Province.h"
#include "../Reforms/Reforms.h"
#include "../Reforms/UncivReforms.h"
#include "../War/War.h"
#include "CultureGroups/Culture.h"
#include "ReligiousGroups/Religion.h"
#include "ReligiousGroups/ReligiousGroup.h"
#include "ReligiousGroups/ReligiousGroups.h"
#include "ModFile.h"
#include <ostream>

namespace V2
{
std::ostream& operator<<(std::ostream& output, const Agreement& agreement);
std::ostream& operator<<(std::ostream& output, const Army& army);
std::ostream& operator<<(std::ostream& output, const Country& country);
std::ostream& operator<<(std::ostream& output, const Factory& factory);
std::ostream& operator<<(std::ostream& output, const Leader& leader);
std::ostream& operator<<(std::ostream& output, const Localisation& localisation);
std::ostream& operator<<(std::ostream& output, const Party& party);
std::ostream& operator<<(std::ostream& output, const Pop& pop);
std::ostream& operator<<(std::ostream& output, const Province& province);
std::ostream& operator<<(std::ostream& output, const Reforms& reforms);
std::ostream& operator<<(std::ostream& output, const Regiment& regiment);
std::ostream& operator<<(std::ostream& output, const Relation& relation);
std::ostream& operator<<(std::ostream& output, const UncivReforms& uncivReforms);
std::ostream& operator<<(std::ostream& output, const ModFile& modFile);
std::ostream& operator<<(std::ostream& output, const War& war);
} // namespace V2

namespace mappers
{
std::ostream& operator<<(std::ostream& output, const PartyType& partyDetails);
std::ostream& operator<<(std::ostream& output, const VersionParser& versionParser);
std::ostream& operator<<(std::ostream& output, const CultureGroups& cultureGroupsMapper);
std::ostream& operator<<(std::ostream& output, const CultureGroup& cultureGroup);
std::ostream& operator<<(std::ostream& output, const Culture& culture);
std::ostream& operator<<(std::ostream& output, const ReligiousGroups& religiousGroupsMapper);
std::ostream& operator<<(std::ostream& output, const ReligiousGroup& religiousGroup);
std::ostream& operator<<(std::ostream& output, const Religion& religion);
} // namespace mappers

#endif // OUTPUT_H