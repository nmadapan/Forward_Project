#include "Visual_feedback.h"

// graphics initialization
void Visual_feedback::InitGraphics ()
{
  // compute size of half edge of cube
  static const float size = CubeSize / 2.0f;

  // setup cube vertex data
  V[0][0] = V[1][0] = V[2][0] = V[3][0] = -size;
  V[4][0] = V[5][0] = V[6][0] = V[7][0] =  size;
  V[0][1] = V[1][1] = V[4][1] = V[5][1] = -size;
  V[2][1] = V[3][1] = V[6][1] = V[7][1] =  size;
  V[0][2] = V[3][2] = V[4][2] = V[7][2] =  size;
  V[1][2] = V[2][2] = V[5][2] = V[6][2] = -size;

  // enable a single OpenGL light. 
  glLightfv (GL_LIGHT0, GL_AMBIENT,  LightAmbient);
  glLightfv (GL_LIGHT0, GL_DIFFUSE,  LightDiffuse);
  glLightfv (GL_LIGHT0, GL_SPECULAR, LightSpecular);
  glLightfv (GL_LIGHT0, GL_POSITION, LightPosition);
  glEnable  (GL_LIGHT0);
  glEnable  (GL_LIGHTING);

  // use depth buffering for hidden surface elimination.
  glEnable (GL_DEPTH_TEST);
}



// window resize callback
void Visual_feedback::RezizeWindow (int w,
              int h)
{
  double glAspect = ((double)w / (double)h);

  glViewport     (0, 0, w, h);
  glMatrixMode   (GL_PROJECTION);
  glLoadIdentity ();
  gluPerspective (60, glAspect, 0.01, 10);

  gluLookAt      (0.3, 0.0, 0.0,
                  0.0, 0.0, 0.0,
                  0.0, 0.0, 1.0);

  glMatrixMode   (GL_MODELVIEW);
  glLoadIdentity ();
}



// scene rendering timer callback
void Visual_feedback::
UpdateDisplay (int val)
{
  if (SimulationOn) glutPostRedisplay ();

  glutTimerFunc (30, UpdateDisplay, 0);
}



// scene rendering callback
void Visual_feedback::
Draw (void)
{
  int            i;
  double         posX, posY, posZ;
  cMatrixGL      mat;
  GLUquadricObj *sphere;
  cVector3d      devicePos;
  cMatrix3d      deviceRot;

  // clean up
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // get info from device
  //dhdGetPosition (&posX, &posY, &posZ);
  //devicePos.set (posX, posY, posZ);
  devicePos.set (0, 0, 0);
  //dhdGetOrientationFrame (deviceRot.m);
  deviceRot.identity();


  // render cube
  mat.set (devicePos, deviceRot);
  mat.glMatrixPushMultiply ();

  glEnable  (GL_COLOR_MATERIAL);
  glColor3f (0.1f, 0.3f, 0.5f);

  for (i=0; i<6; i++) {
    glBegin     (GL_QUADS);
    glNormal3fv (&N[i][0]);
    glVertex3fv (&V[Faces[i][0]][0]);
    glVertex3fv (&V[Faces[i][1]][0]);
    glVertex3fv (&V[Faces[i][2]][0]);
    glVertex3fv (&V[Faces[i][3]][0]);
    glEnd       ();
  }

  mat.glMatrixPop ();

  // render sphere at center of workspace
  glColor3f (0.8f, 0.8f, 0.8f);
  sphere = gluNewQuadric ();
  gluSphere (sphere, 0.005, 64, 64);

  // text overlay
  /*if (ShowRate) {
    if (dhdGetTime() - LastTime > 0.1) {
      Freq     = dhdGetComFreq();
      LastTime = dhdGetTime ();
      sprintf (Perf, "%0.03f kHz", Freq);
    }
    glDisable (GL_LIGHTING);
    glColor3f  (1.0, 1.0, 1.0);
    glRasterPos3f (0.0f, -0.01f, -0.1f);
    for (char *c=Perf; *c != '\0'; c++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    glEnable  (GL_LIGHTING);
  }*/

  glutSwapBuffers ();

  GLenum err = glGetError();
  if (err != GL_NO_ERROR) printf ("error:  %s\n", gluErrorString(err));
}



// keyboard input callback
void Visual_feedback::
Key (unsigned char key,
     int           x,
     int           y)
{
  /*switch (key) {
  case 27:  
  case 'q':
    SimulationOn = false;
    while (!SimulationFinished) dhdSleep (0.01);
    exit(0);
  case 't':
    TorquesON = !TorquesON;
    break;
  case 'f':
    ForcesON = !ForcesON;
    if (ForcesON) dhdEnableForce (DHD_ON);
    else          dhdEnableForce (DHD_OFF);
    break;*/
  }
}



// right-click menu callback
void Visual_feedback::
SetOther (int value)
{
  switch (value) {
  case OPTION_FULLSCREEN:
    glutFullScreen ();
    break;
  case OPTION_WINDOWDISPLAY:
    glutReshapeWindow      (512, 512);
    glutInitWindowPosition (0, 0);
    break;
  }

  glutPostRedisplay ();
}



void* Visual_feedback:: my_GL_Loop (void* pUserData)
{

	 // initialization
	  glutInit (&local_argc, local_argv);
	  int WINDOW_SIZE_W = 512;
	  int WINDOW_SIZE_H = 512;
	  int screenW       = glutGet (GLUT_SCREEN_WIDTH);
	  int screenH       = glutGet (GLUT_SCREEN_HEIGHT);
	  int windowPosX    = (screenW - WINDOW_SIZE_W) / 2;
	  int windowPosY    = (screenH - WINDOW_SIZE_H) / 2;

	  // configure window
	  glutInitWindowSize     (WINDOW_SIZE_W, WINDOW_SIZE_H);
	  glutInitWindowPosition (windowPosX, windowPosY);
	  glutInitDisplayMode    (GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	  glutCreateWindow       (local_argv[0]);
	  glutDisplayFunc        ( Visual_feedback::Draw);
	  glutKeyboardFunc       ( Visual_feedback::Key);
	  glutReshapeFunc        ( Visual_feedback::RezizeWindow);
	  glutSetWindowTitle     ("ISAT - Omega control");

	  // configure right-click menu
	  glutCreateMenu   ( Visual_feedback::SetOther);
	  glutAddMenuEntry ("Full Screen", OPTION_FULLSCREEN);
	  glutAddMenuEntry ("Window Display", OPTION_WINDOWDISPLAY);
	  glutAttachMenu   (GLUT_RIGHT_BUTTON);

	  // initialize graphics scene
	   Visual_feedback::InitGraphics ();


	  // start in window mode
	  glutReshapeWindow      (512, 512);
	  glutInitWindowPosition (0, 0);

	  // update display
	  glutTimerFunc (30, UpdateDisplay, 0);


	  // start main graphic rendering loop
	  //glutMainLoop ();


	 Visual_feedback::glutMainLoop();
	return NULL;
}