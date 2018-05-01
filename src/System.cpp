// System.cpp

#ifndef SYSTEM
#define SYSTEM

#include "System.h"

// Randomizes the state of the passed particle according
// to the bounds of the systes
void System::randomizeParticle(Particle& p) {
    double mass = MAX_MASS * randomFloat();
    p.mass = mass;
    for (int j = 0; j < Particle::D; ++j) {
        p.pos[j] = 2 * bounds * randomFloat() - bounds;
        p.vel[j] = 10 * randomFloat() - 5;
    }
}

// Returns a random float between 0 and 1
double System::randomFloat() {
    float r = rand();
    float rMax = RAND_MAX;
    return r / rMax;
}

// Calculates the center of mass based on the current state of the system
void System::updateCOMS() {
    Particle c;
    double totalMass = 0;
    for (int i = 0; i < particles.size(); i++) {
        totalMass += particles.at(i).mass;
    }
    for (int i = 0; i < particles.size(); i++) {
        Particle p = particles[i];
        for (int d = 0; d < Particle::D; d++) {
            c.pos[d] += p.pos[d] * p.mass / totalMass;
        }
    }
    for (int d = 0; d < Particle::D; d++) {
        history.push_back(c.pos[d]);
    }
    if (history.size() > 1000) {
        for (int d = 0; d < Particle::D; d++) {
            history.pop_front();
        }
    }
    com = c;
}

System::System() {
    // totalMass = -1;
    bounds = 300;
    particles.resize(N);
}

// Initializes the system to a random state
void System::initParticles() {
    srand(time(0));
    history = {};
    // totalMass = 0;
    for (int i = 0; i < particles.size(); i++) {
        randomizeParticle(particles.at(i));
    }
}

// Moves the system timeStep into the future as one step
void System::update(double timeStep) {
    for (int i = 0; i < particles.size(); i++) {
        for (int j = i + 1; j < particles.size(); j++) {
            particles[i].gravitate(particles[j], timeStep);
        }
        particles[i].update(timeStep, bounds);
    }
    updateCOMS();
}

// Adds a random particle to the system
void System::addParticle() {
    Particle p;
    randomizeParticle(p);
    particles.push_back(p);
}

// Removes the particle at index i from the system
void System::removeParticle(int i) {
    if (particles.size() < 1) {
        return;
    }
    if (i < 0) {
        particles.pop_back();
        return;
    }
    particles.at(i) = particles.back();
    particles.pop_back();
}

void System::setBounds(double nBounds) {
    bounds = nBounds;
}


int System::size() const {
    return particles.size();
}

// Returns the dth dimension of the position of particle i
double System::pos(int i, int d) const {
    if (i < 0 || i > size() || d < 0 || d >= Particle::D) {
        return 0;
    }
    return particles.at(i).pos[d];
}

// Returns the dth dimension of the position of the center of mass
double System::comPos(int d) const {
    if (d < 0 || d >= Particle::D) {
        return 0;
    }
    return com.pos[d];
}

// Returns the dth dimension oof the color of particle i
double System::color(int i, int d) const {
    if (i < 0 || i > size() || d < 0 || d > 2) {
        return 0;
    }
    return particles.at(i).color[d];
}

// returns the mass of particle i
double System::mass(int i) const {
    if (i < 0 || i > size()) {
        return 0;
    }
    return particles.at(i).mass;
}


double System::getBounds() const {
    return bounds;
}

// Returns the particle with position closest to the past particle
int System::closestParticle(Particle p) const {
    double minDistance = DBL_MAX;
    int result = -1;
    for (int i = 0; i < particles.size(); i++) {
        double distance = p.distance(particles.at(i));
        if (distance < minDistance) {
            minDistance = distance;
            result = i;
        }
    }
    return result;
}

// Returns the first particle matching the passed color
int System::getByColor(const float mColor[]) const {
    for (int i = 0; i < size(); i++) {
        int c = 0;
        while (c < 3) {
            if (mColor[c] != color(i, c)) {
                break;
            }
            if (c == 2) {
                return i;
            }
            c++;
        }
    }
    return -1;
}

void System::printPositions() const {
    for (int i = 0; i < particles.size(); ++i) {
        Particle p = particles[i];
        std::cout << "Particle " << i << ": Mass = " << p.mass << ", Position = (" << p.pos[0];
        for (int d = 1; d < Particle::D; d++) {
            std::cout << ", " << p.pos[d];
        }
        std::cout << ")" << std::endl;
    }
}

#endif