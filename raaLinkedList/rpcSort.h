#pragma once
#include "raaLinkedList.h"
#include "raaSystem/raaSystem.h"
#include "raaComputerGraphicsAssignment1/raaConstants.h"

raaLinkedList* mergeSortNodeList(raaLinkedList *pllNodeList);
raaLinkedListElement *mergeSort(raaLinkedListElement *plleHead);
raaLinkedListElement* split(raaLinkedListElement *plleHead);
bool compareNodeMass(raaLinkedListElement *plleA, raaLinkedListElement *plleB);
raaLinkedListElement* merge(raaLinkedListElement *plleFront, raaLinkedListElement *plleBack);

void setNodePositionBySortedOrder(unsigned int uiCategories, raaLinkedList *sortedList);
void sortNodesByCategory(unsigned int uiCategory, raaLinkedList *pllSortedList);
void assignNodeToWorldOrderList(raaNode *pNode);
void assignNodeToContinentList(raaNode *pNode);
void assignNodeToCategoryList(unsigned int uiCategory, raaLinkedList *pllSortedList, raaNode *pNode);

void sortNodes(raaLinkedList *pllNodeList);
void performSort(raaLinkedList *pllOriginalNodeList, unsigned int uiNumberOfCategories, raaLinkedList *pllSortedList, nodeFunction *nfSort);