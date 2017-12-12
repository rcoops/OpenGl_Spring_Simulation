#pragma once
#include "raaLinkedList.h"

raaLinkedList* mergeSortNodeList(raaLinkedList *pllNodeList);
raaLinkedListElement *mergeSort(raaLinkedListElement *plleHead);
raaLinkedListElement* split(raaLinkedListElement *plleHead);
bool compareNodeMass(raaLinkedListElement *plleA, raaLinkedListElement *plleB);
raaLinkedListElement* merge(raaLinkedListElement *plleFront, raaLinkedListElement *plleBack);