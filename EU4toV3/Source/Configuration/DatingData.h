#ifndef DATING_DATA
#define DATING_DATA
#include "Date.h"

struct DatingData
{
	date startEU4Date;									 // savegame saved date - conversion date. Can be 1444.11.11, or earlier for conversions.
	date lastEU4Date;										 // last date we see in the save, used to stop conversions and export starting vic3 bookmark.
	const date hardEndingDate = date("1836.1.1"); // We don't convert pops past this date no matter what.
};

#endif // DATING_DATA
