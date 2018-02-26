#include <GL/glut.h>
#include <cmath>
#include <ctime>
#include <list>
#include <iostream>
#include <string.h>
#include "Particle.cpp"
#include "System.cpp"

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

// Used for CoM calculation

// NOTE: Small values may require disabling the optimzation flag.

System cheese;


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

	srand(time(0));
	cheese.initParticles();

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
	if (run) cheese.update(TICK_TIME);
	if (trail) drawHistory();
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
	// Draw cheese.particles
	if (lighting) {
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
	}
	for (int i = 0; i < cheese.N; i++) {
		glColor3f(cheese.particles[i].color[0], cheese.particles[i].color[1], cheese.particles[i].color[2]);
		
		if (planets) {
			glPushMatrix();
			glTranslatef(cheese.particles[i].pos[0] / DISTANCE_SCALE,
			             cheese.particles[i].pos[1] / DISTANCE_SCALE,
			             cheese.particles[i].pos[2] / DISTANCE_SCALE
			            );
			glutSolidSphere(cbrt(cheese.particles[i].mass) / MASS_SCALE, 15, 15);
			glPopMatrix();
		}
		// std::cout << cheese.particles[i].pos[0] << ", "
		//           << cheese.particles[i].pos[1] << ", "
		//           << cheese.particles[i].pos[2] << std::endl;
	}
	if (lighting) {
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
	}
	// Draw center of mass
	if (center) {
		glColor3f(randomFloat(), randomFloat(), randomFloat());
		glPushMatrix();
		glTranslatef(cheese.com.pos[0] / DISTANCE_SCALE,
		             cheese.com.pos[1] / DISTANCE_SCALE,
		             cheese.com.pos[2] / DISTANCE_SCALE
		            );
		glutSolidSphere(0.1, 15, 15);
		glPopMatrix();
	}
}

void drawHistory() {
	auto it = cheese.history.begin();
	for (int i = 0; i < D; ++i) {
		it++;
	}
	int length = 0;
	while (it != cheese.history.end()) {
		double pos[3] = {0, 0, 0};
		for (int i = 0; i < D; i++) {
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
	if (bounds > 0) {
		glColor3f(1, 1, 1);
	} else {
		glColor3f(1, 0, 0);
	}
	glutWireCube(2 * bounds / DISTANCE_SCALE);
}

double randomFloat() {
	return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

void keyboardFunc(unsigned char Key, int x, int y) {
	switch (Key) {
	// ROTATE SYSTEM
	case 'w':	yRot--; break;
	case 'a':	xRot--; break;
	case 's':	yRot++; break;
	case 'd':	xRot++; break;

	// RESIZE SYSTEM
	case 'e':	bounds++; break;
	case 'q':	bounds--; break;

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
			bounds = -1000;
			box = false;
			planets = false;
			trail = true;
			center = false;
		} else if (strcmp(argv[i], "-f") == 0) {
			std::cout << "Initiating fullscreen." << std::endl;
			glutFullScreen();
		} else {
			std::cout << "Unrecognized option: " << argv[i] << std::endl;
		}
	}
}