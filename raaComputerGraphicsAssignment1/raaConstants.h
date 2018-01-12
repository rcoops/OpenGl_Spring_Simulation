#pragma once
#include <string>

//generic
const static unsigned int csg_uiX = 0;
const static unsigned int csg_uiY = 1;
const static unsigned int csg_uiZ = 2;
const static unsigned int csg_uiWidth = 2;
const static unsigned int csg_uiHeight = 3;
const static float csg_fCameraViewAngle = 30.0f;
const static unsigned int csg_uiDefaultStringLength = 128;
const static float csg_fNearClip = 0.1f;
const static float csg_fFarClip = 10000.0f;
const static int csg_uiWindowDefinition[] = { 0,0,512,384 };
const static float csg_fCamAngleSensitivity = 0.0005f;
const static float csg_fCamLinearSensitivity = 0.2f;

const static float csg_fTimeUnit = 0.016667f; // 1 / 60
const static float csg_fDampeningCoefficient = 0.99f;

const static unsigned int csg_uiNumberOfContinents = 6;
const static unsigned int csg_uiNumberOfWorldSystems = 3;

const static unsigned int csg_uiWorldOrdersCategory = 666;
const static unsigned int csg_uiContinentsCategory = 333;

const static float csg_fMinimumNodePosition = 1.0f;
const static float csg_fMaximumNodePosition = 770.0f;

// materials
const static bool csg_bMaterialEmissiveOn = true;
const static bool csg_bMaterialEmissiveOff = false;

const static float csg_afMaterialNeturalColour[] = { 1.0f, 1.0f, 0.1f, 0.6f };
const static float csg_fMaterialNeturalShininess = 15.0f;

const static float csg_afColourTransBrightRed[] = { 1.0f, 0.2f, 0.2f, 0.6f };

const static float csg_afColourClear[] = { 0.6f, 0.7f, 0.8f, 0.7f }; // screen background colour

// continent material colours
const static float csg_afMaterialAfricaColour[] =		{ 1.0f, 0.0f, 0.0f, 1.0f };
const static float csg_afMaterialAsiaColour[] =			{ 0.0f, 1.0f, 0.0f, 1.0f };
const static float csg_afMaterialEuropeColour[] =		{ 0.0f, 0.0f, 1.0f, 1.0f };
const static float csg_afMaterialNorthAmericaColour[] = { 1.0f, 0.0f, 1.0f, 1.0f };
const static float csg_afMaterialOceanaColour[] =		{ 1.0f, 1.0f, 0.0f, 1.0f };
const static float csg_afMaterialSouthAmericaColour[] = { 0.0f, 1.0f, 1.0f, 1.0f };

const float *constantContinentIndexToMaterialColour(int i);

//control
const static unsigned int csg_uiControlDrawGrid = 0;
const static unsigned int csg_uiControlNumFlags = 1;

// parser
std::string constantContinentIndexToName(int i);

const static unsigned int csg_uiParseNetwork = 1;
const static unsigned int csg_uiParseArcs = 2;
const static unsigned int csg_uiParseVector = 3;
const static unsigned int csg_uiParsePartition = 4;

const static unsigned int csg_uiParseXCoord = 1;
const static unsigned int csg_uiParseGDP = 2;

const static unsigned int csg_uiParseContinent = 1;
const static unsigned int csg_uiParseWorldSystem = 2;
const static float csg_afParseLayoutScale[] = { 800.0f, 800.0f, 800.0f };
const static float csg_fParseDefaultMass = 100.0f;
const static float csg_fParseDefaultSize = 1.0f;

// grid
const static float csg_afDisplayListGridColour[] = { 0.3f, 0.3f, 0.1f, 0.3f };
const static float csg_fDisplayListGridMin = -500.0f;
const static float csg_fDisplayListGridMax = 500.0f;
const static float csg_fDisplayListGridSpace = 50.0f;
static unsigned int gs_uiGridDisplayList = 0;

static unsigned int gs_uiBaseNodeDisplayListId = 0;

// menu
static unsigned int gs_uiMainMenu = 0;
static unsigned int gs_uiSortingSubMenu = 0;
static unsigned int gs_uiMovementMenu = 0;

const enum menuOption
{
	positionByContinent,
	positionByWorldSystem,
	positionBySpringSolver,

	positionRandom,
	pausePositioning,
	increaseSpeed,
	decreaseSpeed,

	toggleGrid,
	toggleCamCentre,
	toggleHud
};

// move multipliers

const static float csg_fMinSpeed = 0.125f;
const static float csg_fMaxSpeed = 8.0f;
const static float csg_fSpeedMultiplier = 2.0f;
