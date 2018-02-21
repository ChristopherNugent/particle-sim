#include <GL/glut.h>
#include <cmath>
#include <ctime>
#include <iostream>
#include "Particle.cpp"


// Display refresh time
int refreshMills = 5;

// Simulation seconds between state updates
// (large values might cause strange behavior)
int tickTime = 50;

// Tick counter for view rotation
int tick = 1;

// Used for CoM calculation
double totalMass = -1;

const int N = 10;
Particle particles[N];


void display(void);
void reshape(int x, int y);
double randomFloat();
void initParticles();
void drawParticles();
void update(double timeStep);
void timer(int value);


int main(int argc, char **argv)
{

	glutInit(&argc, argv);
	glutInitWindowSize(550, 550);
	glutCreateWindow("Sphere");
	glEnable(GL_DEPTH_TEST);   // Enable depth testing for z-culling
	glDepthFunc(GL_LEQUAL);    // Set the type of depth-test

	if (argc > 1) {
		// LIGHTING SHENANIGANS
		GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
		GLfloat mat_shininess[] = { 50.0 };
		GLfloat light_position[] = { 1, 0, 	0 };
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
		glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);

		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		// END LIGHTING SHENANIGANS
	}
	// END OPENGL SETUP

	srand(time(0));
	initParticles();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutTimerFunc(0, timer, 0);
	glutMainLoop();
	return 0;
}


void display(void)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glTranslatef(0, 0, -15.5);
	glRotatef(30, 1, 0, 0);
	glRotatef(tick / 15.0, 0, 1, 0);
	glRotatef(tick / 15.0, 1, 0, 0);
	glRotatef(tick / 15.0, 0, 0, 1);
	// glRotatef(90, 1, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
	update(tickTime);
	glColor3f(1, 1, 1);
	glutWireCube(6);
	drawParticles();
	glFlush();
	tick++;
}

void reshape(int x, int y)
{
	if (y == 0 || x == 0) return;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(39.0, (GLdouble)x / (GLdouble)y, 0.6, 21.0);
	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, x, y);  //Use the whole window for rendering
}

// Called back when timer expired [NEW]
void timer(int value) {
	glutPostRedisplay();      // Post re-paint request to activate display()
	glutTimerFunc(refreshMills, timer, 0); // next timer call milliseconds later
}


//*****************************************************************************
//  START SIMULATION RELATED CODE
//*****************************************************************************


double randomFloat() {
	double pos = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	return pos;
}

void initParticles() {
	totalMass = 0;
	for (int i = 0; i < N; i++) {
		double mass = 100000 * randomFloat();
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
	for (int i = 0; i < N; i++)
	{
		glColor3f(particles[i].color[0], particles[i].color[1], particles[i].color[2]);
		glPushMatrix();
		double pos[3] = {0, 0, 0};
		for (int j = 0; j < D; ++j)
		{
			pos[j] = particles[i].pos[j];
			cm[j] += (particles[i].mass / totalMass) * pos[j];
		}
		glTranslatef(pos[0] / 100, pos[1] / 100, pos[2] / 100);
		glutSolidSphere(cbrt(particles[i].mass) / 100, 15, 15);
		glPopMatrix();
		// std::cout << particles[i].pos[0] << ", "
		//           << particles[i].pos[1] << ", "
		//           << particles[i].pos[2] << std::endl;
	}
	// Draw center of mass
	glColor3f(randomFloat(), randomFloat(), randomFloat());
	glPushMatrix();
	glTranslatef(cm[0] / 100, cm[1] / 100, cm[2] / 100);
	glutSolidSphere(0.1, 15, 15);
	glPopMatrix();
}

void update(double timeStep) {
	for (int i = 0; i < N; i++) {		// include all particles
		for (int j = i + 1; j < N; j++) {
			particles[i].gravitate(particles[j], timeStep);
		}
		// blackHole.gravitate(particles[i], timeStep);
		particles[i].update(timeStep);
	}
	// particles[0].xPos = 0;
	// particles[0].yPos = 0;
	// particles[0].zPos = 0;
}
