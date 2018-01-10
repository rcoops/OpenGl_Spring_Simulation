#include <windows.h>
#include <mmsystem.h>

#include "raaSystem/raaSystem.h"
#include "raaConstants.h"
#include "raaLinkedList/rpcSort.h"

enum nodePositioning
{
	none, springs, worldOrder, continent
};

// Spring calc
void calculateSpringForce(raaArc *pArc);
void calculateNodeMotion(raaNode *pNode);
void resetNodeForce(raaNode *pNode);

// Sort movement
void togglePositioning(nodePositioning positioning);
void calculateNodeMovement(raaSystem *pSystem);
void moveToSortedOrder(float *vfNewPosition, raaNode *pNode);
void moveToWorldOrderPositions(raaNode *pNode);
void moveToContinentPositions(raaNode *pNode);

void pauseMovement();