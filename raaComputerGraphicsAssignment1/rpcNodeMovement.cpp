#include "rpcNodeMovement.h"

nodePositioning g_eCurrentNodePositioning = none;
nodePositioning g_eSavedPreviousPositioning = none;

float g_afAverageNodePosition[4];
float g_afAggregatedPositions[4];
float g_fNumberOfNodes = 0.0f;
float g_fSpeedMultiplier = 1.0f;

/* no need for these functions to be available in other files */

// Spring calc
void calculateSpringForce(raaArc *pArc);
void calculateNodeMotion(raaNode *pNode);
void resetNodeForce(raaNode *pNode);

// Sort movement
void moveToSortedOrder(float *vfNewPosition, raaNode *pNode);
void moveToWorldOrderPositions(raaNode *pNode);
void moveToContinentPositions(raaNode *pNode);

void aggregatePosition(raaNode *pNode);

/* CONTROLS */

void togglePositioning(nodePositioning npPositioning)
{
	g_eCurrentNodePositioning = g_eCurrentNodePositioning == npPositioning ? none : npPositioning; // toggle current positioning on/off
}

void calculateNodeMovement(raaSystem *pSystem)
{
	switch (g_eCurrentNodePositioning)
	{
	case springs:
		visitNodes(pSystem, resetNodeForce);
		visitArcs(pSystem, calculateSpringForce);
		visitNodes(pSystem, calculateNodeMotion);
		break;
	case worldOrder:
		visitNodes(pSystem, moveToWorldOrderPositions);
		break;
	case continent:
		visitNodes(pSystem, moveToContinentPositions);
		break;
	case none:
		break;
	}
}

/* SPRINGS */

void resetNodeForce(raaNode *pNode)
{
	vecInitDVec(pNode->m_vfForce);
}

void calculateSpringForce(raaArc *pArc)
{
	float vfArc[4], vfArcDirection[4], vfForce[4];
	vecInitDVec(vfArc); vecInitDVec(vfArcDirection); vecInitDVec(vfForce);

	vecSub(pArc->m_pNode0->m_afPosition, pArc->m_pNode1->m_afPosition, vfArc); // Calc arc length vector

	float fCurrentArcLength = vecNormalise(vfArc, vfArcDirection); // Calc scalar distance and direction vector
	float fCurrentArcExtension = (fCurrentArcLength - pArc->m_fIdealLen) / pArc->m_fIdealLen; // Calc spring extension
	float fForce = fCurrentArcExtension * pArc->m_fSpringCoef; // Calc spring force

	vecScalarProduct(vfArcDirection, fForce, vfForce);
	vecSub(pArc->m_pNode0->m_vfForce, vfForce, pArc->m_pNode0->m_vfForce);
	vecAdd(pArc->m_pNode1->m_vfForce, vfForce, pArc->m_pNode1->m_vfForce);
}

void calculateNodeMotion(raaNode *pNode)
{
	float vfAcceleration[4], vfDisplacement[4];
	vecInitDVec(vfAcceleration); vecInitDVec(vfDisplacement);

	vecScalarProduct(pNode->m_vfForce, 1 / pNode->m_fMass, vfAcceleration); // a = f/m
						
	for (int i = 0; i < 3; ++i) // s = vt + (at^2)/2
	{
		vfDisplacement[i] = (pNode->m_vfVelocity[i] * csg_fTimeUnit) + vfAcceleration[i] * pow(csg_fTimeUnit, 2) * 0.5f * g_fSpeedMultiplier;
	}

	vecAdd(pNode->m_afPosition, vfDisplacement, pNode->m_afPosition); // p' = p + s

	vecCopy(vecScalarProduct(vfDisplacement, 1.0f / csg_fTimeUnit, vfDisplacement), pNode->m_vfVelocity, 3);

	// Apply damping v^''=v'*(1.0-∑_0^1 DampingCoef)
	vecScalarProduct(pNode->m_vfVelocity, csg_fDampeningCoefficient, pNode->m_vfVelocity);
}

/* SORTING */

void moveToSortedOrder(float *afNewPosition, raaNode *pNode)
{
	float vfRoute[4], vfDirection[4];
	vecInitDVec(vfRoute); vecInitDVec(vfDirection);

	vecSub(pNode->m_afPosition, afNewPosition, vfRoute); // Calc route from original to target position
	float fCurrentDistance = vecNormalise(vfRoute, vfDirection); // Calc scalar distance and direction vector

	if (fCurrentDistance > 1) // If it's not really close
	{
		if (fCurrentDistance > g_fSpeedMultiplier) // can move by multiplier if wont overshoot
		{
			vecScalarProduct(vfDirection, g_fSpeedMultiplier, vfDirection); // multiply by speed
		}
		vecSub(pNode->m_afPosition, vfDirection, pNode->m_afPosition); // Move the node position a single unit in direction
	}
}

void moveToWorldOrderPositions(raaNode *pNode)
{
	moveToSortedOrder(pNode->m_afWorldOrderPosition, pNode);
}

void moveToContinentPositions(raaNode *pNode)
{
	moveToSortedOrder(pNode->m_afContinentPosition, pNode);
}

void aggregatePosition(raaNode *pNode)
{
	vecAdd(g_afAggregatedPositions, pNode->m_afPosition, g_afAggregatedPositions);
}

float* calculateAveragePosition(raaSystem *pSystem)
{
	if (!g_fNumberOfNodes) g_fNumberOfNodes = (int) count(&(pSystem->m_llNodes));
	vecInitDVec(g_afAggregatedPositions); vecInitDVec(g_afAverageNodePosition); // reset all to 0

	visitNodes(pSystem, aggregatePosition); // add up all node positions

	return vecScalarProduct(g_afAggregatedPositions, 1.0f / g_fNumberOfNodes, g_afAverageNodePosition); // return average
}

/* MOVEMENT CONTROL */

char* getSpeed()
{
	char sSpeed[16];
	sprintf(sSpeed, "Speed: x%.3f\n", g_fSpeedMultiplier);
	return sSpeed;
}

void pauseMovement()
{
	if (g_eCurrentNodePositioning == none) // go back to original setting
	{
		g_eCurrentNodePositioning = g_eSavedPreviousPositioning;
	}
	else // save original setting and wipe
	{
		g_eSavedPreviousPositioning = g_eCurrentNodePositioning;
		g_eCurrentNodePositioning = none;
	}
}

void increaseMovementSpeed()
{
	if (g_fSpeedMultiplier < csg_fMaxSpeed)
	{
		g_fSpeedMultiplier = g_fSpeedMultiplier * csg_fSpeedMultiplier;
	}
}

void decreaseMovementSpeed()
{
	if (g_fSpeedMultiplier > csg_fMinSpeed)
	{
		g_fSpeedMultiplier = g_fSpeedMultiplier / csg_fSpeedMultiplier;
	}

}

void randomisePosition(raaNode *pNode)
{
	vecRand(csg_fMinimumNodePosition, csg_fMaximumNodePosition, pNode->m_afPosition);
}
