#include "raaSpringCalc.h"

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

	// s = vt + (at^2)/2
	for (int i = 0; i < 3; ++i)
	{
		vfDisplacement[i] = (pNode->m_vfVelocity[i] * csg_fTimeUnit) + vfAcceleration[i] * pow(csg_fTimeUnit, 2) * 0.5f;
	}
	
	vecAdd(pNode->m_afPosition, vfDisplacement, pNode->m_afPosition); // p' = p + s

	vecCopy(vecScalarProduct(vfDisplacement, 1.0f / csg_fTimeUnit, vfDisplacement), pNode->m_vfVelocity, 3);

	// Apply damping v^''=v'*(1.0-∑_0^1 DampingCoef)
	vecScalarProduct(pNode->m_vfVelocity, csg_fDampeningCoefficient, pNode->m_vfVelocity);
}
