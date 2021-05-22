#include "Regiment.h"

V2::Regiment::Regiment(REGIMENTTYPE _regimentType) : regimentType(_regimentType)
{
	switch (regimentType)
	{
	case REGIMENTTYPE::irregular:
	case REGIMENTTYPE::infantry:
	case REGIMENTTYPE::cavalry:
	case REGIMENTTYPE::artillery:
		isShip = false;
		break;
	case REGIMENTTYPE::manowar:
	case REGIMENTTYPE::frigate:
	case REGIMENTTYPE::clipper_transport:
		isShip = true;
	}
}
