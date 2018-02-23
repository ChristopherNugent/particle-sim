#include <GL/glut.h>
#include <cmath>
#include <ctime>
#include <iostream>
#include <list>
#include "Particle.cpp"


// Display refresh time
const int REFRESH_MS = 5;

// Simulation seconds between state updates
// (large values might cause strange behavior)
const int TICK_TIME = 5;

// Scalers. Larger values means smaller picture.
const double DISTANCE_SCALE = pow(10, 2);
const double MASS_SCALE = pow(10, 4);
const double MAX_MASS = pow(10, 11);
const double Z_DIST = 15.5;

std::list<double> history;


// Tick counter for view rotation
int tick = 1;
int xRot = 0;
int yRot = 0;
bool run = true;
bool lighting = false;
bool trail = true;
bool box = true;

// Used for CoM calculation
double totalMass = -1;

// NOTE: Small values may require disabling the optimzation flag.
const int N = 15;
Particle particles[N];


void display(void);
void reshape(int x, int y);
void timer(int value);
void keyboardFunc(unsigned char Key, int x, int y);

double randomFloat();
void initParticles();
void drawParticles();
void drawHistory();
void update(double timeStep);


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
	GLfloat light_position[] = { 1, 0, 	0 };
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	// END LIGHTING SHENANIGANS
	// END OPENGL SETUP

	srand(time(0));
	initParticles();

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
	// glLoadIdentity();
	glPushMatrix();

	glTranslatef(0, 0, -Z_DIST);

	glRotatef(30, 1, 0, 0);

	glRotatef(yRot, 1, 0, 0);
	glRotatef(xRot, 0, 1, 0);

	glColor3f(1, 1, 1);

	if (box) glutWireCube(2 * bounds / DISTANCE_SCALE);
	if (run) update(TICK_TIME);
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


void initParticles() {
	history = {};
	totalMass = 0;
	for (int i = 0; i < N; i++) {
		double mass = MAX_MASS * randomFloat();
		particles[i].mass = mass;
		totalMass += mass;
		for (int i = 0; i < D; ++i) {
			particles[i].pos[i] = 40 * randomFloat() - 20;
			particles[i].vel[i] = 10 * randomFloat() - 5;
		}
	}
}

void drawParticles() {
	double cm[3] = {0, 0, 0};
	// Draw particles
	if (lighting) {
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
	}
	for (int i = 0; i < N; i++) {
		glColor3f(particles[i].color[0], particles[i].color[1], particles[i].color[2]);
		glPushMatrix();
		double pos[3] = {0, 0, 0};
		for (int j = 0; j < D; ++j)	{
			pos[j] = particles[i].pos[j];
			cm[j] += (particles[i].mass / totalMass) * pos[j];
		}
		glTranslatef(pos[0] / DISTANCE_SCALE,
		             pos[1] / DISTANCE_SCALE,
		             pos[2] / DISTANCE_SCALE
		            );
		glutSolidSphere(cbrt(particles[i].mass) / MASS_SCALE, 15, 15);
		glPopMatrix();
		// std::cout << particles[i].pos[0] << ", "
		//           << particles[i].pos[1] << ", "
		//           << particles[i].pos[2] << std::endl;
	}
	if (lighting) {
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
	}
	// Draw center of mass
	glColor3f(randomFloat(), randomFloat(), randomFloat());
	glPushMatrix();
	glTranslatef(cm[0] / DISTANCE_SCALE,
	             cm[1] / DISTANCE_SCALE,
	             cm[2] / DISTANCE_SCALE
	            );
	glutSolidSphere(0.1, 15, 15);
	glPopMatrix();
	if (run) {
		for (int i = 0; i < D; ++i) {
			history.push_back(cm[i]);
			if (history.size() > 1000) {
				for (int i = 0; i < D; ++i) {
					history.pop_front();
				}
			}
		}
	}
}

void drawHistory() {
	auto it = history.begin();
	for (int i = 0; i < D; ++i) {
		it++;
	}
	int length = 0;
	while (it != history.end()) {
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
		glColor3f(1, 1, 1);
		glutSolidCube(0.03);
		glPopMatrix();
		length++;
	}
}

void update(double timeStep) {
	for (int i = 0; i < N; i++) {		// include all particles
		for (int j = i + 1; j < N; j++) {
			particles[i].gravitate(particles[j], timeStep);
		}
		// blackHole.gravitate(particles[i], timeStep);
		particles[i].update(timeStep);
	}
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

	// PAUSE
	case ' ':	run = !run;	break;

	// EXIT
	case 27:	exit(1); break;
	};
}