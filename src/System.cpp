#ifndef SYSTEM
#define SYSTEM

#include "System.h"

System::System() {
    totalMass = -1;
    bounds = 300;
}

void System::initParticles() {
    history = {};
    totalMass = 0;
    for (int i = 0; i < N; i++) {
        double mass = MAX_MASS * randomFloat();
        particles[i].mass = mass;
        totalMass += mass;
        for (int j = 0; j < Particle::D; ++j) {
            particles[i].pos[j] = 40 * randomFloat() - 20;
            particles[i].vel[j] = 10 * randomFloat() - 5;
        }
    }
}

void System::updateCOMS() {
    Particle c;
    for (int i = 0; i < N; i++) {
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

void System::update(double timeStep) {
    updateCOMS();
    for (int i = 0; i < N; i++) {
        for (int j = i + 1; j < N; j++) {
            particles[i].gravitate(particles[j], timeStep);
        }
        particles[i].update(timeStep, bounds);
    }
}

double System::randomFloat() {
    return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

void System::printPositions() {
    for (int i = 0; i < N; ++i) {
        Particle p = particles[i];
        std::cout << "Particle " << i << ": Mass = " << p.mass << ", Position = (" << p.pos[0];
        for (int d = 1; d < Particle::D; d++) {
            std::cout << ", " << p.pos[d];
        }
        std::cout << ")" << std::endl;
    }
}

#endif