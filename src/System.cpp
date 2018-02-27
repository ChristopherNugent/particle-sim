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
    for (int group = 0; group < NUM_GROUPS; group++) {      // per group
        Particle p;
        p.setColor(1, 0, 0);
        double groupMass = 0;
        // sum total mass
        for (int j = group * GROUP_SIZE; j < (group + 1) * GROUP_SIZE && j < N; j++) {
            groupMass += particles[j].mass;
        }
        p.mass = groupMass;
        // calc position
        for (int j = group * GROUP_SIZE; j < (group + 1) * GROUP_SIZE && j < N; j++) {
            for (int d = 0; d < Particle::D; d++) {
                p.pos[d] += particles[j].pos[d] * particles[j].mass / groupMass;
            }
        }
        coms[group] = p;
        for (int d = 0; d < Particle::D; d++) {
            c.pos[d] += p.pos[d] * p.mass / totalMass;
        }
    }
    com = c;
    for (int d = 0; d < Particle::D; d++) {
        history.push_back(c.pos[d]);

        if (history.size() > 1000) {
            for (int i = 0; i < Particle::D; ++i) {
                history.pop_front();
            }
        }
    }
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

