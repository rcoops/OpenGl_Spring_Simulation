#include "raaSort.h"

raaLinkedList g_pllNodeByWorldOrder[csg_uiWorldOrders];
raaLinkedList g_pllNodeByContinent[csg_uiContinents];

/**
 * Adapted from http://www.geeksforgeeks.org/merge-sort-for-doubly-linked-list/
 */
raaLinkedList* mergeSortNodeList(raaLinkedList *pllNodeList)
{
	// No list, no elements or 1 item = no sorting
	if (pllNodeList && pllNodeList->m_pHead && pllNodeList->m_pHead->m_pNext)
	{
		pllNodeList->m_pHead = mergeSort(pllNodeList->m_pHead);
		// Fudge to reset our tail
		for (raaLinkedListElement *pE = pllNodeList->m_pHead; pE; pE = pE->m_pNext)
		{
			pllNodeList->m_pTail = pE;
		}
	}
	return pllNodeList;
}

raaLinkedListElement *mergeSort(raaLinkedListElement *plleHead)
{
	if (!plleHead || !plleHead->m_pNext)
	{
		return plleHead;
	}
	raaLinkedListElement *plleSecond = split(plleHead);

	plleHead = mergeSort(plleHead);
	plleSecond = mergeSort(plleSecond);

	return merge(plleHead, plleSecond);
}

raaLinkedListElement* split(raaLinkedListElement *plleHead)
{
	raaLinkedListElement *plleSplit = plleHead;

	if (plleHead && plleHead->m_pNext)
	{
		raaLinkedListElement *plleSlow;
		raaLinkedListElement *plleFast = plleSlow = plleHead;

		while (plleFast->m_pNext && plleFast->m_pNext->m_pNext)
		{
			plleFast = plleFast->m_pNext->m_pNext;
			plleSlow = plleSlow->m_pNext;
		}
		plleSplit = plleSlow->m_pNext;
		plleSlow->m_pNext = 0;
	}
	return plleSplit;
}

bool compareNodeMass(raaLinkedListElement *plleA, raaLinkedListElement *plleB)
{
	return ((raaNode*) plleA->m_pData)->m_fMass >= ((raaNode*) plleB->m_pData)->m_fMass;
}

raaLinkedListElement* merge(raaLinkedListElement *plleFirst, raaLinkedListElement *plleSecond)
{
	if (!plleFirst)
	{
		return  plleSecond;
	}
	if (!plleSecond)
	{
		return plleFirst;
	}
	if(compareNodeMass(plleFirst, plleSecond))
	{
		plleFirst->m_pNext = merge(plleFirst->m_pNext, plleSecond);
		plleFirst->m_pNext->m_pLast = plleFirst;
		plleFirst->m_pLast = 0;
		return plleFirst;
	}
	plleSecond->m_pNext = merge(plleFirst, plleSecond->m_pNext);
	plleSecond->m_pNext->m_pLast = plleSecond;
	plleSecond->m_pLast = 0;
	return plleSecond;
}

void assignNodeToWorldOrderList(raaNode *pNode)
{
	assignNodeToCategoryList(pNode->m_uiWorldSystem, g_pllNodeByWorldOrder, pNode);
}

void assignNodeToContinentList(raaNode *pNode)
{
	assignNodeToCategoryList(pNode->m_uiContinent, g_pllNodeByContinent, pNode);
}

void assignNodeToCategoryList(unsigned int uiCategory, raaLinkedList *pllSortedList, raaNode *pNode)
{
	if (pllSortedList && pNode)
	{
		// Category can be used as array index for array of category lists
		pushTail(&pllSortedList[uiCategory - 1], initElement(new raaLinkedListElement, pNode, csg_uiNode));
	}
}

void setNodePositionBySortedOrder(unsigned int uiNumberOfCategories, raaLinkedList *pllSortedList)
{
	for (int i = 0; i < uiNumberOfCategories; ++i)
	{
		float fX = 100.0f * (i - 1);
		float *afPosition;
		for (raaLinkedListElement *pE = pllSortedList[i].m_pHead; pE; pE = pE->m_pNext)
		{
			raaNode *pNode = (raaNode*)pE->m_pData;
			raaNode *pLast = pE->m_pLast ? ((raaNode*)pE->m_pLast->m_pData) : 0;
			float fYPosition = pLast ? afPosition[1] + pLast->m_fTextOffset + 30.0f : 50.0f; // Use 'last' position to find y (if last exists)
			afPosition = uiNumberOfCategories == 6 ? pNode->m_afContinentPosition : pNode->m_afWorldOrderPosition;
			afPosition[0] = fX;
			afPosition[1] = fYPosition;
			afPosition[2] = 300.0f;
		}
	}
}

void sortNodesByCategory(unsigned int uiCategory, raaLinkedList *pllOriginalNodeList)
{
	raaLinkedList *pllSortedList = 0;
	unsigned int uiNumberOfCategories = 0;
	nodeFunction *nfSort = 0;
	switch (uiCategory)
	{
	case csg_uiWorldOrdersCategory:
		pllSortedList = g_pllNodeByWorldOrder;
		uiNumberOfCategories = csg_uiWorldOrders;
		nfSort = assignNodeToWorldOrderList;
		break;
	case csg_uiContinentsCategory:
		pllSortedList = g_pllNodeByContinent;
		uiNumberOfCategories = csg_uiContinents;
		nfSort = assignNodeToContinentList;
		break;
	}

	for (int i = 0; i < uiNumberOfCategories; ++i) initList(&(pllSortedList)[i], csg_uiNode);

	visitNodesInList(pllOriginalNodeList, nfSort);

	for (int i = 0; i < uiNumberOfCategories; ++i) mergeSortNodeList(&pllSortedList[i]);

	setNodePositionBySortedOrder(uiNumberOfCategories, pllSortedList);
}

void moveToSortedOrder(float *afNewPosition, raaNode *pNode)
{
	float vfRoute[4], vfDirection[4];
	vecInitDVec(vfRoute); vecInitDVec(vfDirection);
	vecSub(pNode->m_afPosition, afNewPosition, vfRoute); // Calc route from original to target position
	float fCurrentDistance = vecNormalise(vfRoute, vfDirection); // Calc scalar distance and direction vector
	if (fCurrentDistance > 1)
	{
		vecSub(pNode->m_afPosition, vfDirection, pNode->m_afPosition); // Calc arc length vector
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

void sortNodes(raaLinkedList *pllNodeList)
{
	sortNodesByCategory(csg_uiWorldOrdersCategory, pllNodeList);
	sortNodesByCategory(csg_uiContinentsCategory, pllNodeList);
}
