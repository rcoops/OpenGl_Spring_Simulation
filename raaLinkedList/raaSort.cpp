#include "raaSort.h"

raaLinkedList g_pllNodeByWorldOrder[csg_uiNumberOfWorldSystems];
raaLinkedList g_pllNodeByContinent[csg_uiNumberOfContinents];

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

/* end */

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
		float *afPosition; // No need to initialise - will just point to continent or world order position on node
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

void performSort(raaLinkedList *pllOriginalNodeList, unsigned int uiNumberOfCategories, raaLinkedList *pllSortedList, nodeFunction *nfSort)
{
	for (int i = 0; i < uiNumberOfCategories; ++i) initList(&(pllSortedList)[i], csg_uiNode);

	visitNodesInList(pllOriginalNodeList, nfSort);

	for (int i = 0; i < uiNumberOfCategories; ++i) mergeSortNodeList(&pllSortedList[i]);

	setNodePositionBySortedOrder(uiNumberOfCategories, pllSortedList);
}

void sortNodesByCategory(unsigned int uiCategory, raaLinkedList *pllOriginalNodeList)
{
	raaLinkedList *pllSortedList = 0;
	nodeFunction *nfSort = 0;
	unsigned int uiNumberOfCategories = 0;

	switch (uiCategory)
	{
	case csg_uiWorldOrdersCategory:
		pllSortedList = g_pllNodeByWorldOrder;
		uiNumberOfCategories = csg_uiNumberOfWorldSystems;
		nfSort = assignNodeToWorldOrderList;
		break;
	case csg_uiContinentsCategory:
		pllSortedList = g_pllNodeByContinent;
		uiNumberOfCategories = csg_uiNumberOfContinents;
		nfSort = assignNodeToContinentList;
		break;
	}

	performSort(pllOriginalNodeList, uiNumberOfCategories, pllSortedList, nfSort);
}

void sortNodes(raaLinkedList *pllNodeList)
{
	sortNodesByCategory(csg_uiWorldOrdersCategory, pllNodeList);
	sortNodesByCategory(csg_uiContinentsCategory, pllNodeList);
}
