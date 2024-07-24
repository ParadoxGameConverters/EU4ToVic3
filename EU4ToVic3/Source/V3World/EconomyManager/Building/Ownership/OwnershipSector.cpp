#include "OwnershipSector.h"

V3::OwnershipSector::OwnershipSector(std::istream& theStream)
{
}

void V3::OwnershipSector::registerKeys()
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}
