#include <GL/glut.h>
#include <string.h>
#include <iostream>
#include <cmath>
#include "System.h"

// Display refresh time
const int REFRESH_MS = 5;

// Simulation seconds between state updates
// (large values might cause strange behavior)
const int TICK_TIME = 5;
int tick = 1;

// Scalers. Larger values means smaller picture.
const double DISTANCE_SCALE = pow(10, 2);
const double MASS_SCALE = pow(10, 4);
const double Z_DIST = 15.5;



// Settings adjustible from keyboard
int xRot = 0;
int yRot = 0;
bool run = true;
bool lighting = false;
bool trail = true;
bool box = true;
bool planets = true;
bool center = true;
bool debug = false;

// Used for CoM calculation

// NOTE: Small values may require disabling the optimzation flag.

System pSys;


void display(void);
void reshape(int x, int y);
void timer(int value);
void keyboardFunc(unsigned char Key, int x, int y);

double randomFloat();
void initParticles();
void drawParticles();
void drawHistory();
void drawBox();
void update(double timeStep);
void parseArgs(int argc, char **argv);

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitWindowSize(550, 550);
	glutCreateWindow("Gravity Simulation");
	glEnable(GL_DEPTH_TEST);   // Enable depth testing for z-culling
	glDepthFunc(GL_LEQUAL);    // Set the type of depth-test

	glutKeyboardFunc(keyboardFunc);

	// LIGHTING SHENANIGANS
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 50.0 };
	GLfloat light_position[] = { 1, 1, 	0.5 };
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	// END LIGHTING SHENANIGANS
	// END OPENGL SETUP

	parseArgs(argc, argv);

	pSys.initParticles();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutTimerFunc(0, timer, 0);
	glutMainLoop();
	return 0;
}


void display(void) {
	glMatrixMode(GL_MODELVIEW);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glPushMatrix();
	glTranslatef(0, 0, -Z_DIST);
	glRotatef(30, 1, 0, 0);
	glRotatef(yRot, 1, 0, 0);
	glRotatef(xRot, 0, 1, 0);

	if (box) drawBox();
	if (run) pSys.update(TICK_TIME);
	if (trail) drawHistory();
	if (debug) pSys.printPositions();
	drawParticles();

	glFlush();
	glPopMatrix();
	tick++;
}

void reshape(int x, int y) {
	if (y == 0 || x == 0) return;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(39.0, (GLdouble)x / (GLdouble)y, 0.6, 31.0);
	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, x, y);  //Use the whole window for rendering
}

// Called back when timer expired [NEW]
void timer(int value) {
	glutPostRedisplay();      // Post re-paint request to activate display()
	glutTimerFunc(REFRESH_MS, timer, 0); // next timer call milliseconds later
}


//*****************************************************************************
//  START SIMULATION RELATED CODE
//*****************************************************************************



void drawParticles() {
	// Draw pSys.particles
	if (lighting) {
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
	}
	for (int i = 0; i < pSys.size(); i++) {
		glColor3f(pSys.particles[i].color[0], pSys.particles[i].color[1], pSys.particles[i].color[2]);

		if (planets) {
			glPushMatrix();
			glTranslatef(pSys.particles[i].pos[0] / DISTANCE_SCALE,
			             pSys.particles[i].pos[1] / DISTANCE_SCALE,
			             pSys.particles[i].pos[2] / DISTANCE_SCALE
			            );
			glutSolidSphere(cbrt(pSys.particles[i].mass) / MASS_SCALE, 15, 15);
			glPopMatrix();
		}
	}
	if (lighting) {
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
	}
	// Draw center of mass
	if (center) {
		glColor3f(randomFloat(), randomFloat(), randomFloat());
		glPushMatrix();
		glTranslatef(pSys.com.pos[0] / DISTANCE_SCALE,
		             pSys.com.pos[1] / DISTANCE_SCALE,
		             pSys.com.pos[2] / DISTANCE_SCALE
		            );
		glutSolidSphere(0.1, 15, 15);
		glPopMatrix();
	}
}

void drawHistory() {
	auto it = pSys.history.begin();
	for (int i = 0; i < Particle::D; ++i) {
		it++;
	}
	int length = 0;
	while (it != pSys.history.end()) {
		double pos[3] = {0, 0, 0};
		for (int i = 0; i < Particle::D; i++) {
			pos[i] = *it;
			it++;
		}
		glPushMatrix();
		glTranslatef(pos[0] / DISTANCE_SCALE,
		             pos[1] / DISTANCE_SCALE,
		             pos[2] / DISTANCE_SCALE
		            );
		glColor3f(0.5, 0.5, 0.5);
		glColor3f(randomFloat(), randomFloat(), randomFloat());
		glutSolidCube(0.03);
		glPopMatrix();
		length++;
	}
}

void drawBox() {
	if (pSys.bounds > 0) {
		glColor3f(1, 1, 1);
	} else {
		glColor3f(1, 0, 0);
	}
	glutWireCube(2 * pSys.bounds / DISTANCE_SCALE);
}

double randomFloat() {
	float r = rand();
	float rMax = RAND_MAX;
	return r / rMax;
}

void keyboardFunc(unsigned char Key, int x, int y) {
	switch (Key) {
	// ROTATE SYSTEM
	case 'w':	yRot--; break;
	case 'a':	xRot--; break;
	case 's':	yRot++; break;
	case 'd':	xRot++; break;

	// RESIZE SYSTEM
	case 'e':	pSys.bounds++; break;
	case 'q':	pSys.bounds--; break;

	// VISUAL TOGGLES
	case 'b':	box = !box; 			break;
	case 'l':	lighting = !lighting; 	break;
	case 't':	trail = !trail; 		break;
	case 'p':	planets = !planets;		break;
	case 'c':	center = !center;		break;

	// PAUSE
	case ' ':	run = !run;	break;

	// EXIT
	case 27:	exit(1); break;
	};
}

void parseArgs(int argc, char **argv) {
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-s") == 0) {
			std::cout << "Starting in starfield." << std::endl;
			pSys.bounds = -1000;
			box = false;
			planets = false;
			trail = true;
			center = false;
		} else if (strcmp(argv[i], "-f") == 0) {
			std::cout << "Initiating fullscreen." << std::endl;
			glutFullScreen();
		} else if (strcmp(argv[i], "-d") == 0) {
			debug = true;
		} else {
			std::cout << "Unrecognized option: " << argv[i] << std::endl;
		}
	}
}