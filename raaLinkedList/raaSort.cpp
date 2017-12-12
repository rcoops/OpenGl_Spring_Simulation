#include "raaSort.h"
#include "raaSystem/raaSystem.h"

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
