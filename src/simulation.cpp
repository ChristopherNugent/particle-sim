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
void mouseFunc(int button, int state, int x, int y);

double randomFloat();
void initParticles();
void drawParticles();
void drawHistory();
void drawBox();
void update(double timeStep);
void parseArgs(int argc, char **argv);
void removeParticleAt(int x, int y);
void printWelcome();

int main(int argc, char **argv) {
	printWelcome();
	glutInit(&argc, argv);
	glutInitWindowSize(550, 550);
	glutCreateWindow("Gravity Simulation");
	glEnable(GL_DEPTH_TEST);   // Enable depth testing for z-culling
	glDepthFunc(GL_LEQUAL);    // Set the type of depth-test

	glutKeyboardFunc(keyboardFunc);
	glutMouseFunc(mouseFunc);
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
		glColor3f(pSys.color(i, 0), pSys.color(i, 1), pSys.color(i, 2));

		if (planets) {
			glPushMatrix();
			glTranslatef(pSys.pos(i, 0) / DISTANCE_SCALE,
			             pSys.pos(i, 1) / DISTANCE_SCALE,
			             pSys.pos(i, 2) / DISTANCE_SCALE
			            );
			glutSolidSphere(cbrt(pSys.mass(i)) / MASS_SCALE, 15, 15);
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
		glTranslatef(pSys.comPos(0) / DISTANCE_SCALE,
		             pSys.comPos(1) / DISTANCE_SCALE,
		             pSys.comPos(2) / DISTANCE_SCALE
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
	if (pSys.getBounds() > 0) {
		glColor3f(1, 1, 1);
	} else {
		glColor3f(1, 0, 0);
	}
	glutWireCube(2 * pSys.getBounds() / DISTANCE_SCALE);
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
	case 'e':	pSys.setBounds(pSys.getBounds() + 1); break;
	case 'q':	pSys.setBounds(pSys.getBounds() - 1); break;


	case 'n':	pSys.addParticle();		break;
	case 'm':	pSys.removeParticle();	break;
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
			pSys.setBounds(-1000);
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

void mouseFunc(int button, int state, int x, int y) {
	if (state == GLUT_DOWN) {
		switch (button) {
		case GLUT_LEFT_BUTTON:
			removeParticleAt(x, y);
			break;
		};
	}
}

void removeParticleAt(int x, int y) {
	std::vector< unsigned char > pixels(3);
	glReadPixels(
	    x, glutGet( GLUT_WINDOW_HEIGHT ) - y,
	    1, 1,
	    GL_RGB, GL_UNSIGNED_BYTE, &pixels[0]
	);
	float color[3] = {0, 0, 0};
	for (int i = 0; i < 3; i++) {
		color[i] = (int)pixels[i] / 255.0;
	}
	std::cout << "Color: " << color[0] << ", " << color[1] << ", " << color[2] << std::endl;
	int i = pSys.getByColor(color);
	std::cout << "Target: particle " << i << " at "
	          << "(" << pSys.pos(i, 0) << ", " << pSys.pos(i, 1) << ", " << pSys.pos(i, 2) << ")" << std::endl << std::endl;
	if (i > -1) {
		pSys.removeParticle(i);
	}
}

void printWelcome() {
	std::cout << "_________________________________________________________________________________" << std::endl
	          << "Developed by Chris Nugent for CSC313 - Graphics and Visual Computing             |" << std::endl
	          << "                                                                                 |" << std::endl
	          << "Welcome to a gravity simulator!                                                  |" << std::endl
	          << "                                                                                 |" << std::endl
	          << "There a few buttons you can use to affect the simulation:                        |" << std::endl
	          << "Spacebar - pause/resume simulation                                               |" << std::endl
	          << "     n/m - add/remove particle. You can also click on a particle to remove it.   |" << std::endl
	          << "    wasd - rotate view                                                           |" << std::endl
	          << "     q/e - shrink/grow box - interesting things happen when negative...          |" << std::endl
	          << "       t - toggle draw trail                                                     |" << std::endl
	          << "       b - toggle draw box                                                       |" << std::endl
	          << "       p - toggle draw particles                                                 |" << std::endl
	          << "       c - toggle draw center of mass                                            |" << std::endl
	          << "_________________________________________________________________________________|" << std::endl
	          << std::endl;
	;
}