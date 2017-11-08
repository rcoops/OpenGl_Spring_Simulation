#include "raaConstants.h"

std::string constantContinentIndexToName(int i) 
{
	switch (i)
	{
		case 1: return "Africa";
		case 2: return "Asia";
		case 3: return "Europe";
		case 4: return "North America";
		case 5: return "Oceania";
		case 6: return "South America";
		default: return "Unknown";
	}
}

const float *constantContinentIndexToMaterialColour(int i)
{
	const float *paContinentMaterial = csg_afMaterialNeturalColour;
	switch (i)
	{
	case 1: paContinentMaterial = csg_afMaterialAfricaColour;
		break;
	case 2: paContinentMaterial = csg_afMaterialAsiaColour;
		break;
	case 3: paContinentMaterial = csg_afMaterialEuropeColour;
		break;
	case 4: paContinentMaterial = csg_afMaterialNorthAmericaColour;
		break;
	case 5: paContinentMaterial = csg_afMaterialOceanaColour;
		break;
	case 6: paContinentMaterial = csg_afMaterialSouthAmericaColour;
		break;
	}
	return paContinentMaterial;
}