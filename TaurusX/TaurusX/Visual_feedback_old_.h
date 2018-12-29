#pragma once

#include <math.h>
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>

#include "CMaths.h"
#include "CMacrosGL.h"

#if defined(LINUX) || defined(MACOSX)
#include <pthread.h>
#endif

#ifdef MACOSX
#include "GLUT/glut.h"
#else
#include "GL/glut.h"
#endif


// window display options
const int OPTION_FULLSCREEN     = 1;
const int OPTION_WINDOWDISPLAY  = 2;

// object stiffness
double Stiffness   = 2012;
double TorqueGain  =    1.0;

// set size of cube (half edge)
const float CubeSize = 0.06f;

// status flags
bool SimulationOn;
bool SimulationFinished;
// int  Status[DHD_MAX_STATUS];
bool HapticsON = false;
bool TorquesON = true;
bool ForcesON  = true;

// text overlay globals
double LastTime;
double Freq;
char   Perf[50];
bool   ShowRate = true;

// white diffuse light
GLfloat LightAmbient[]  = {0.5f, 0.5f, 0.5f, 1.0f};
GLfloat LightDiffuse[]  = {0.8f, 0.8f, 0.8f, 1.0f};
GLfloat LightSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};

// light source position
GLfloat LightPosition[] = {1.0f, 0.5f, 0.8f, 0.0f};

// normals for the 6 faces of a cube
GLfloat N[6][3]   = { {-1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {1.0, 0.0, 0.0},
                      {0.0, -1.0, 0.0}, {0.0, 0.0, 1.0}, {0.0, 0.0, -1.0} };

// vertex indices for the 6 faces of a cube
GLint Faces[6][4] = { {0, 1, 2, 3}, {3, 2, 6, 7}, {7, 6, 5, 4},
                      {4, 5, 1, 0}, {5, 6, 2, 1}, {7, 4, 0, 3} };

// will be filled in with X,Y,Z vertices
GLfloat V[8][3];

class Visual_feedback {
public:
	int local_argc;
	char **local_argv;
	Visual_feedback()
	{
	}
	void InitGraphics ();
	// window resize callback
	void RezizeWindow (int w,int h);
	void UpdateDisplay (int val);

	// scene rendering callback
	void Draw (void);

	// keyboard input callback
	void Key (unsigned char key,int x,int y);

	// right-click menu callback
	void SetOther (int value);

	void* my_GL_Loop (void* pUserData);

};







