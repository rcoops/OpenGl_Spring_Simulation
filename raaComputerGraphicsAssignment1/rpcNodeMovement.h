#include <windows.h>
#include <mmsystem.h>

#include "raaSystem/raaSystem.h"
#include "raaConstants.h"
#include "raaLinkedList/rpcSort.h"

enum nodePositioning
{
	none, springs, worldOrder, continent
};

// Sort movement
void togglePositioning(nodePositioning positioning);
void calculateNodeMovement(raaSystem *pSystem);

void pauseMovement();

float* calculateAveragePosition(raaSystem *pSystem);
void randomisePosition(raaNode *pNode);

float  increaseMovementSpeed();
float  decreaseMovementSpeed();