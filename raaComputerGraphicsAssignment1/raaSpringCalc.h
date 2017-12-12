#include <windows.h>
#include <mmsystem.h>

#include "raaSystem/raaSystem.h"
#include "raaConstants.h"

void calculateSpringForce(raaArc *pArc);
void calculateNodeMotion(raaNode *pNode);
void resetNodeForce(raaNode *pNode);
void calcTime();