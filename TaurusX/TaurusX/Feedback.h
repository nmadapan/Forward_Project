#ifndef __FEEDBACK_VISUAL__
#define __FEEDBACK_VISUAL__

// For openGL
#include "GL/gl.h"
#include "GL/glut.h"
#include "GL/glu.h"

#include "Basic.h"

void render_text(double x, double y, char *string)
{
	int len, i;
	glRasterPos2f(GLfloat(x), GLfloat(y));
	len = (int) strlen(string);
	for (i = 0; i < len; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);
	}

}

void setup() {  
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f); 
}

void display() { 
	std::vector <GLfloat> d; // d is a temp var to store height of bar, used in display()
	d.resize(2);
	Point3D report_point, input_point;
	taurus.Read_Pose_Packet();
	double warning_threshold;
	double dampling_factor;

	if (tele_operating_flag)
	{
		for (size_t arm = 0; arm<2; arm++) 
		{
			input_point.set_point(taurus.pose_packet.TipPose[arm][0][3], taurus.pose_packet.TipPose[arm][1][3], taurus.pose_packet.TipPose[arm][2][3]);
			pthread_mutex_lock(&mutexHaptic);
			report_point.set_point(haptic_packet.TipPose[arm][0][3], haptic_packet.TipPose[arm][1][3], haptic_packet.TipPose[arm][2][3]);
			pthread_mutex_unlock(&mutexHaptic);

			haptic_init_flag = true; // comment this line if user wants to enter the value, if not commented, use the default value
			if (haptic_init_flag == false)
			{
				std::cout<<" Enter warning threshold value: "<<std::endl;
				cin>>warning_threshold;
				seed[0].set_warning_threshold(warning_threshold);
				seed[1].set_warning_threshold(warning_threshold);
				std::cout<<" Enter dampling value: "<<std::endl;
				cin>>dampling_factor;
				seed[0].set_damping_factor(dampling_factor);
				seed[1].set_damping_factor(dampling_factor);
				haptic_init_flag = true;
			}

			seed[arm].feed_both(input_point, report_point);
			d[arm] = (GLfloat) seed[arm].calculate_warning_level(Haptic_Seed::VERBOSE); 

		}
	}
	else 
	{
		d[0] = 0;	
		d[1] = 0;	
	}

	double beep_level = max(d[0], d[1]);
	//std::cout<<beep_level<<std::endl;
	if (beep_level > 0.3 ) // I manually set this threshold, because when there is tiny warning, no Beep is needed. 
		Beep(DWORD(523 + beep_level*5000), DWORD(10)); // warning_level is from 0 to 1, then it's 2523 Hz 

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(0.0f, 0.0f, 0.0f);

	// draw the background text
	render_text(-0.3, 0.9, "Left Gripper");
	render_text(+0.05, 0.9, "Right Gripper");

	// draw the axis
	glBegin(GL_LINES);

	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(0.0f,-1.0f);
	glVertex2f(0.0f, 1.0f);

	glEnd();

	// draw the bar chart
	glBegin(GL_QUADS);

	// left hand
	glColor3f(abs(d[0]), 1-abs(d[0]), 0.0f); 
	glVertex2f(-0.7f, -1 + 2 * d[0]); // vertex 1 // plus -1 to shift it downwards to the bottom of the screen, 2*d because d varies from 0 to 1 and the bar varies from -1 to 1
	glVertex2f(-0.7f, -1.0f); // vertex 2
	glVertex2f(-0.3f, -1.0f); // vertex 3
	glVertex2f(-0.3f, -1 + 2 * d[0]); // vertex 

	// right hand
	glColor3f(abs(d[1]), 1-abs(d[1]), 0.0f); 
	glVertex2f(0.3f, -1 + 2*d[1]); // vertex 1
	glVertex2f(0.3f, -1.0f); // vertex 2
	glVertex2f(0.7f, -1.0f); // vertex 3
	glVertex2f(0.7f, -1 + 2*d[1]); // vertex 

	glEnd();

	//glFlush();  // Render now

	glutSwapBuffers(); 
}

void* Visual_Feedback(void* info)
{
	// initialize the OpenGL 
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(800,600);
	//glutInitWindowPosition(0, 0);
	glutCreateWindow("Taurus control");

	setup();
	glutDisplayFunc(display);
	glutIdleFunc(display);
	//glutTimerFunc(25, update, 0);
	glutMainLoop();
	//pthread_exit(NULL);
	return 0;
}


#endif