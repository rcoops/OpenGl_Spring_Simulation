#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glut.h>
#include <map>
#include <conio.h>

#include <raaCamera/raaCamera.h>
#include <raaUtilities/raaUtilities.h>
#include <raaMaths/raaMaths.h>
#include <raaMaths/raaVector.h>
#include <raaSystem/raaSystem.h>
#include <raaPajParser/raaPajParser.h>
#include <raaText/raaText.h>
#include <raaLinkedList/rpcSort.h>

#include "raaConstants.h"
#include "raaParse.h"
#include "raaControl.h"
#include "rpcNodeMovement.h"

float g_afAverageNodePosition[4];
float g_afAggregatedPositions[4];
float g_fNumberOfNodes = 0.0f;

// core system global data
raaCameraInput g_Input; // structure to hadle input to the camera comming from mouse/keyboard events
raaCamera g_Camera; // structure holding the camera position and orientation attributes
raaSystem g_System; // data structure holding the imported graph of data - you may need to modify and extend this to support your functionallity
raaControl g_Control; // set of flag controls used in my implmentation to retain state of key actions

// global var: parameter name for the file to load
const static char csg_acFileParam[] = { "-input" };

// global var: file to load data from
char g_acFile[256];

// core functions -> reduce to just the ones needed by glut as pointers to functions to fulfill tasks
void display(); // The rendering function. This is called once for each frame and you should put rendering code here
void idle(); // The idle function is called at least once per frame and is where all simulation and operational code should be placed
void reshape(int iWidth, int iHeight); // called each time the window is moved or resived
void keyboard(unsigned char c, int iXPos, int iYPos); // called for each keyboard press with a standard ascii key
void keyboardUp(unsigned char c, int iXPos, int iYPos); // called for each keyboard release with a standard ascii key
void sKeyboard(int iC, int iXPos, int iYPos); // called for each keyboard press with a non ascii key (eg shift)
void sKeyboardUp(int iC, int iXPos, int iYPos); // called for each keyboard release with a non ascii key (eg shift)
void mouse(int iKey, int iEvent, int iXPos, int iYPos); // called for each mouse key event
void motion(int iXPos, int iYPos); // called for each mouse motion event
void processMainMenuSelection(int iMenuItem);

// Non glut functions
void initMenu();
void processPositioningMenuSelection(int iMenuItem);
void processMainMenuSelection(int iMenuItem);
void myInit(); // the myinit function runs once, before rendering starts and should be used for setup
void nodeDisplay(raaNode *pNode); // callled by the display function to draw nodes
void nodeTextDisplay(raaNode *pNode);
void arcDisplay(raaArc *pArc); // called by the display function to draw arcs
void buildGrid(); // 

float* calculateAveragePosition();
void aggregatePosition(raaNode *pNode);
void randomisePosition(raaNode *pNode);

// Node init functions
void toggleNodeSorting(nodePositioning positioning);
void setMaterialColourByContinent(raaNode *pNode);
void drawShapeDependentOnWorldSystem(raaNode *pNode);
void setNodeDimensionByWorldOrder(raaNode *pNode);
void initNodeDisplayLists();
void initNodeDisplayList(raaNode *pNode);

/* POSITIONING */

void aggregatePosition(raaNode *pNode)
{
	vecAdd(g_afAggregatedPositions, pNode->m_afPosition, g_afAggregatedPositions);
}

float* calculateAveragePosition()
{
	vecInitDVec(g_afAggregatedPositions); vecInitDVec(g_afAverageNodePosition); // reset all to 0

	visitNodes(&g_System, aggregatePosition); // add up all node positions

	return vecScalarProduct(g_afAggregatedPositions, 1.0f / g_fNumberOfNodes, g_afAverageNodePosition); // return average
}

void randomisePosition(raaNode *pNode)
{
	vecRand(csg_fMinimumNodePosition, csg_fMaximumNodePosition, pNode->m_afPosition);
}

void toggleNodeSorting(nodePositioning npPositioning)
{
	g_Camera.m_bIsCentred = true;
	togglePositioning(npPositioning);
}

/* DISPLAY FUNCTIONS */

void nodeDisplay(raaNode *pNode) // function to render a node (called from display())
{
	glPushMatrix();

	glTranslatef(pNode->m_afPosition[0], pNode->m_afPosition[1], pNode->m_afPosition[2]);
	nodeTextDisplay(pNode);
	glCallList(gs_uiBaseNodeDisplayListId + pNode->m_uiId - 1);

	glPopMatrix();
}

void nodeTextDisplay(raaNode *pNode) // function to render node name (called from display())
{
	glPushAttrib(GL_ALL_ATTRIB_BITS); // push attribute state to enable constrained state changes
	glPushMatrix();

	glTranslatef(0.0f, pNode->m_fTextOffset, 0.0f);
	setMaterialColourByContinent(pNode);
	glMultMatrixf(camRotMatInv(g_Camera)); // always face cam
	glScalef(10.0f, 10.0f, 1.0f);
	outlinePrint(pNode->m_acName);

	glPopMatrix();
	glPopAttrib();
}

void arcDisplay(raaArc *pArc) // function to render an arc (called from display())
{
	glPushMatrix();

	glColor4f(0.0f, 1.0f, 0.0f, csg_fLineOpacity);
	glVertex3fv(pArc->m_pNode0->m_afPosition);

	glColor4f(1.0f, 0.0f, 0.0f, csg_fLineOpacity);
	glVertex3fv(pArc->m_pNode1->m_afPosition);

	glPopMatrix();
}

// draw the scene. Called once per frame and should only deal with scene drawing (not updating the simulator)
void display()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT); // clear the rendering buffers
		// switch back to modelview mode
	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity(); // clear the current transformation state
	glMultMatrixf(camObjMat(g_Camera)); // apply the current camera transform

	// draw the grid if the control flag for it is true	
	if (controlActive(g_Control, csg_uiControlDrawGrid)) glCallList(gs_uiGridDisplayList);

	glPushAttrib(GL_ALL_ATTRIB_BITS); // push attribute state to enable constrained state changes
	visitNodes(&g_System, nodeDisplay); // loop through all of the nodes and draw them with the nodeDisplay function
	glPopAttrib();

	glPushAttrib(GL_ALL_ATTRIB_BITS); // push attrib marker
	glDisable(GL_LIGHTING); // switch of lighting to render lines
	glBegin(GL_LINES);
	visitArcs(&g_System, arcDisplay); // loop through all of the arcs and draw them with the arcDisplay function
	glEnd();
	glPopAttrib();
	glFlush(); // ensure all the ogl instructions have been processed

	glutSwapBuffers(); // present the rendered scene to the screen
}

void idle() // processing of system and camera data outside of the rendering loop
{
	calculateNodeMovement(&g_System);
	if (g_Camera.m_bIsCentred) // Centre cam on average node position
	{
		camCentre(g_Camera, calculateAveragePosition());
	}

	controlChangeResetAll(g_Control); // re-set the update status for all of the control flags
	camProcessInput(g_Input, g_Camera); // update the camera pos/ori based on changes since last render
	camResetViewportChanged(g_Camera); // re-set the camera's viwport changed flag after all events have been processed
	glutPostRedisplay(); // ask glut to update the screen
}

void reshape(int iWidth, int iHeight) // respond to a change in window position or shape
{
	/* want to know what proportion of the screen (iWidth iHeight) our mouse exists in */
	glViewport(0, 0, iWidth, iHeight);  // re-size the rendering context to match window
	camSetViewport(g_Camera, 0, 0, iWidth, iHeight); // inform the camera of the new rendering context size
	glMatrixMode(GL_PROJECTION); // switch to the projection matrix stack 
	glLoadIdentity(); // clear the current projection matrix state
	gluPerspective(csg_fCameraViewAngle, ((float)iWidth) / ((float)iHeight), csg_fNearClip, csg_fFarClip); // apply new state based on re-sized window
	glMatrixMode(GL_MODELVIEW); // swap back to the model view matrix stac
	glGetFloatv(GL_PROJECTION_MATRIX, g_Camera.m_afProjMat); // get the current projection matrix and sort in the camera model
	glutPostRedisplay(); // ask glut to update the screen
}

/* CONTROL */

void keyboard(unsigned char c, int iXPos, int iYPos) // detect key presses and assign them to actions
{
	switch (c)
	{
	case 'w':
		camInputTravel(g_Input, tri_pos); // mouse zoom
		break;
	case 's':
		camInputTravel(g_Input, tri_neg); // mouse zoom
		break;
	case 'c':
		camPrint(g_Camera); // print the camera data to the comsole
		break;
	case 'g':
		controlToggle(g_Control, csg_uiControlDrawGrid); // toggle the drawing of the grid
		break;
	case 'r':
		toggleNodeSorting(springs);
		break;
	case 'n':
		toggleNodeSorting(worldOrder);
		break;
	case 'm':
		toggleNodeSorting(continent);
		break;
	case 't':
		visitNodes(&g_System, randomisePosition); // deliberate fallthrough to pause movement
	case 'b':
		pauseMovement();
		break;
	case 'z':
		g_Camera.m_bIsCentred = !g_Camera.m_bIsCentred;
	default:;//nothing
	}
}

void keyboardUp(unsigned char c, int iXPos, int iYPos) // detect standard key releases
{
	switch (c)
	{
	case 'w':
	case 's':
		camInputTravel(g_Input, tri_null); // end the camera zoom action
		break;
	}
}

void sKeyboard(int iC, int iXPos, int iYPos)
{
	// detect the pressing of arrow keys for mouse zoom and record the state for processing by the camera
	switch (iC)
	{
	case GLUT_KEY_UP:
		camInputTravel(g_Input, tri_pos);
		break;
	case GLUT_KEY_DOWN:
		camInputTravel(g_Input, tri_neg);
		break;
	}
}

void sKeyboardUp(int iC, int iXPos, int iYPos)
{
	// detect when mouse zoom action (arrow keys) has ended
	switch (iC)
	{
	case GLUT_KEY_UP:
	case GLUT_KEY_DOWN:
		camInputTravel(g_Input, tri_null);
		break;
	}
}

void mouse(int iKey, int iEvent, int iXPos, int iYPos)
{
	// capture the mouse events for the camera motion and record in the current mouse input state
	if (iKey == GLUT_LEFT_BUTTON)
	{
		camInputMouse(g_Input, (iEvent == GLUT_DOWN) ? true : false);
		if (iEvent == GLUT_DOWN) camInputSetMouseStart(g_Input, iXPos, iYPos);
	}
	else if (iKey == GLUT_MIDDLE_BUTTON)
	{
		g_Camera.m_bIsCentred = false;
		camInputMousePan(g_Input, (iEvent == GLUT_DOWN) ? true : false);
		if (iEvent == GLUT_DOWN) camInputSetMouseStart(g_Input, iXPos, iYPos);
	}
}

void motion(int iXPos, int iYPos)
{
	// if mouse is in a mode that tracks motion pass this to the camera model
	if (g_Input.m_bMouse || g_Input.m_bMousePan) camInputSetMouseLast(g_Input, iXPos, iYPos);
}

/* INIT */

void myInit()
{
	controlInit(g_Control); // setup my event control structure
	
	initMaths(); // initalise the maths library

	initMenu(); // initialise menu & submenus

	// opengl setup - this is a basic default for all rendering in the render loop
	glClearColor(csg_afColourClear[0], csg_afColourClear[1], csg_afColourClear[2], csg_afColourClear[3]); // set the window background colour
	glEnable(GL_DEPTH_TEST); // enables occusion of rendered primatives in the window
	glEnable(GL_LIGHT0); // switch on the primary light
	glEnable(GL_LIGHTING); // enable lighting calculations to take place
	glEnable(GL_BLEND); // allows transparency and fine lines to be drawn
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // defines a basic transparency blending mode
	glEnable(GL_NORMALIZE); // normalises the normal vectors used for lighting - you may be able to switch this iff (performance gain) is you normalise all normals your self
	glEnable(GL_CULL_FACE); // switch on culling of unseen faces
	glCullFace(GL_BACK); // set culling to not draw the backfaces of primatives
		
	buildGrid();	// build the grid display list - display list are a performance optimization 

	// initialise the data system and load the data file
	initSystem(&g_System);
	parse(g_acFile, parseSection, parseNetwork, parseArc, parsePartition, parseVector);

	// Calc average node position for camera centre
	g_fNumberOfNodes = (float) count(&g_System.m_llNodes);

	// One-time operations for nodes
	visitNodes(&g_System, setNodeDimensionByWorldOrder);
	sortNodes(&(g_System.m_llNodes));
	initNodeDisplayLists();

	// Camera setup
	camInit(g_Camera); // initalise the camera model
	camInputInit(g_Input); // initialise the persistant camera input data 
	camInputExplore(g_Input, true); // define the camera navigation mode
	camCentre(g_Camera, calculateAveragePosition());
}

void initNodeDisplayLists()
{
	if (!gs_uiBaseNodeDisplayListId) gs_uiBaseNodeDisplayListId = glGenLists((int)g_fNumberOfNodes);
	visitNodes(&g_System, initNodeDisplayList);
}

void initNodeDisplayList(raaNode *pNode)
{
	glNewList(gs_uiBaseNodeDisplayListId + pNode->m_uiId - 1, GL_COMPILE);
	setMaterialColourByContinent(pNode);
	drawShapeDependentOnWorldSystem(pNode);
	glEndList();
}

void setMaterialColourByContinent(raaNode *pNode)
{
	utilitiesColourToMat(constantContinentIndexToMaterialColour(pNode->m_uiContinent), 2.0f);
}

void setNodeDimensionByWorldOrder(raaNode *pNode)
{
	switch (pNode->m_uiWorldSystem)
	{
	case 1:
		pNode->m_fDimension = mathsRadiusOfSphereFromVolume(pNode->m_fMass);
		pNode->m_fTextOffset = pNode->m_fDimension + 5.0f;
		break;
	case 2:
		pNode->m_fDimension = mathsDimensionOfCubeFromVolume(pNode->m_fMass);
		pNode->m_fTextOffset = pNode->m_fDimension / 2.0f + 5.0f;
		break;
	case 3:
		pNode->m_fDimension = mathsRadiusOfConeFromVolume(pNode->m_fMass);
		pNode->m_fTextOffset = pNode->m_fDimension + 5.0f;
		break;
	default: /* If it doesn't have a world system allocation, it's not a country... */;
	}
}

void drawShapeDependentOnWorldSystem(raaNode *pNode)
{
	switch (pNode->m_uiWorldSystem)
	{
	case 1:
		glutSolidSphere(pNode->m_fDimension, 10, 10);
		break;
	case 2:
		glutSolidCube(pNode->m_fDimension);
		break;
	case 3:
		glTranslatef(0.0f, -pNode->m_fDimension, 0.0f); // Adjust position to allow rotation
		glRotatef(270.0f, 1.0f, 0.0f, 0.0f);			// Rotate so cone pointing up
		glutSolidCone(pNode->m_fDimension, pNode->m_fDimension * 2, 10, 10);
		break;
	default: /* If it doesn't have a world system allocation, it's not a country... */;
	}
}

void buildGrid()
{
	if (!gs_uiGridDisplayList) gs_uiGridDisplayList = glGenLists(1); // create a display list

	glNewList(gs_uiGridDisplayList, GL_COMPILE); // start recording display list

	glPushAttrib(GL_ALL_ATTRIB_BITS); // push attrib marker
	glDisable(GL_LIGHTING); // switch of lighting to render lines

	glColor4fv(csg_afDisplayListGridColour); // set line colour

	glBegin(GL_LINES);						// draw the grid lines
	for (int i = (int)csg_fDisplayListGridMin; i <= (int)csg_fDisplayListGridMax; i++)
	{
		glVertex3f(((float)i)*csg_fDisplayListGridSpace, 0.0f, csg_fDisplayListGridMin*csg_fDisplayListGridSpace);
		glVertex3f(((float)i)*csg_fDisplayListGridSpace, 0.0f, csg_fDisplayListGridMax*csg_fDisplayListGridSpace);
		glVertex3f(csg_fDisplayListGridMin*csg_fDisplayListGridSpace, 0.0f, ((float)i)*csg_fDisplayListGridSpace);
		glVertex3f(csg_fDisplayListGridMax*csg_fDisplayListGridSpace, 0.0f, ((float)i)*csg_fDisplayListGridSpace);
	}
	glEnd(); // end line drawing

	glPopAttrib(); // pop attrib marker (undo switching off lighting)

	glEndList(); // finish recording the displaylist
}

/* MENUS */

void initMenu()
{
	gs_uiPositioningSubMenu = glutCreateMenu(processPositioningMenuSelection);
	glutAddMenuEntry("Position By Continent", positionByContinent);
	glutAddMenuEntry("Position By World System", positionByWorldSystem);
	glutAddMenuEntry("Toggle Spring Solver", positionBySpringSolver);
	glutAddMenuEntry("Randomise Positions", positionRandom);
	glutAddMenuEntry("Toggle Pause", pausePositioning);

	gs_uiMainMenu = glutCreateMenu(processMainMenuSelection);
	glutAddSubMenu("Sort & Solve", gs_uiPositioningSubMenu);
	glutAddMenuEntry("Toggle Grid View", toggleGrid);
	glutAddMenuEntry("Toggle Camera Centre", toggleCamCentre);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void processPositioningMenuSelection(int iMenuItem)
{
	switch (iMenuItem)
	{
	case positionByContinent:
		toggleNodeSorting(continent);
		break;
	case positionByWorldSystem:
		toggleNodeSorting(worldOrder);
		break;
	case positionBySpringSolver:
		toggleNodeSorting(springs);
		break;
	case positionRandom:
		visitNodes(&g_System, randomisePosition); // fall-through to prevent further positioning
	case pausePositioning:
		pauseMovement();
		break;
	}
}

void processMainMenuSelection(int iMenuItem)
{
	switch (iMenuItem)
	{
	case toggleGrid:
		controlToggle(g_Control, csg_uiControlDrawGrid);
		break;
	case toggleCamCentre:
		g_Camera.m_bIsCentred = !g_Camera.m_bIsCentred;
		break;
	}
}

/* MAIN */

int main(int argc, char* argv[])
{
	// check parameters to pull out the path and file name for the data file
	for (int i = 0; i<argc; i++) if (!strcmp(argv[i], csg_acFileParam)) sprintf_s(g_acFile, "%s", argv[++i]);

	if (strlen(g_acFile)) // if there is a data file
	{
		glutInit(&argc, (char**)argv); // start glut (opengl window and rendering manager)

		glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA); // define buffers to use in ogl
		glutInitWindowPosition(csg_uiWindowDefinition[csg_uiX], csg_uiWindowDefinition[csg_uiY]);  // set rendering window position
		glutInitWindowSize(csg_uiWindowDefinition[csg_uiWidth], csg_uiWindowDefinition[csg_uiHeight]); // set rendering window size
		glutCreateWindow("raaAssignment1-2017 - Cooper, R");  // create rendering window and give it a name

		buildFont(); // setup text rendering (use outline print function to render 3D text

		myInit(); // application specific initialisation

		// provide glut with callback functions to enact tasks within the event loop
		glutDisplayFunc(display);
		glutIdleFunc(idle);
		glutReshapeFunc(reshape);
		glutKeyboardFunc(keyboard);
		glutKeyboardUpFunc(keyboardUp);
		glutSpecialFunc(sKeyboard);
		glutSpecialUpFunc(sKeyboardUp);
		glutMouseFunc(mouse);
		glutMotionFunc(motion);
		glutMainLoop(); // start the rendering loop running, this will only ext when the rendering window is closed 

		killFont(); // cleanup the text rendering process
		glutDestroyMenu(gs_uiMainMenu);
		glutDestroyMenu(gs_uiPositioningSubMenu);

		return 0; // return a null error code to show everything worked
	}
	
	// if there isn't a data file 
	printf("The data file cannot be found, press any key to exit...\n");
	_getch();
	return 1; // error code
}
