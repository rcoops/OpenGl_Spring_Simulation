#include "raaSystem/raaSystem.h"
#include "raaConstants.h"

void calculateSpringForce(raaArc *pArc)
{
	float afArcDistance[4], afArcDirection[4], afForce[4];
	vecInitDVec(afArcDistance);
	vecInitDVec(afArcDirection);
	vecInitDVec(afForce);

	vecSub(pArc->m_pNode0->m_afPosition, pArc->m_pNode1->m_afPosition, afArcDistance); // Calc arc length vector

	float fArcDistance = vecNormalise(afArcDistance, afArcDirection); // Calc scalar distance and direction vector
	float fExtension = (fArcDistance - pArc->m_fIdealLen) / pArc->m_fIdealLen; // Calc spring extension
	float fForce = fExtension * pArc->m_fSpringCoef; // Calc spring force

	vecScalarProduct(afArcDirection, fForce, afForce);
	vecSub(pArc->m_pNode0->m_afForce, afForce, pArc->m_pNode0->m_afForce);
	vecAdd(pArc->m_pNode1->m_afForce, afForce, pArc->m_pNode1->m_afForce);
}

void calculateNodeMotion(raaNode *pNode)
{
	float afAcceleration[4], afDisplacement[4];
	float timeSquared = csg_fTimeUnit * csg_fTimeUnit;
	vecInitDVec(afAcceleration);
	vecInitDVec(afDisplacement);
	// Calc accel due to spring force a = f/m
	vecScalarProduct(pNode->m_afForce, 1.0f / pNode->m_fMass, afAcceleration);

	// Calculate displacement
	for (int i = 0; i < 3; i++) {
		afDisplacement[i] = (pNode->m_afVelocity[i] * csg_fTimeUnit) + (0.5f * afAcceleration[i] * timeSquared);
	}
	
	//vecScalarProduct(pNode->m_afVelocity, csg_fTimeUnit, afVelocityTimesTime);
	// 1/2 acceleration * time squared
	//vecScalarProduct(afAcceleration, timeSquared * 0.5f, afAcceleration);
	// displacement
	//vecAdd(afVelocityTimesTime, afAcceleration, afDisplacement);

	vecAdd(pNode->m_afPosition, afDisplacement, pNode->m_afPosition);  // Add displacement to body
	vecScalarProduct(afDisplacement, 1.0f / csg_fTimeUnit, pNode->m_afVelocity);
	vecScalarProduct(pNode->m_afVelocity, 0.95, pNode->m_afVelocity);
}