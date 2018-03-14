#ifndef PARTICLE
#define PARTICLE

#include "Particle.h"

Particle::Particle() {
    for (int d = 0; d < D; ++d) {
        pos[d] = 0;
        vel[d] = 0;
    }
    mass = 0;
    for (int i = 0; i < 3; ++i) {
        color[i] = rand() % 255 / 255.0;
    }

};

void Particle::update(double timeStep, double bounds) {
    for (int i = 0; i < D; ++i)
    {
        /* code */

        if (pos[i] > bounds && vel[i] > 0) {
            vel[i] *= -REBOUND;
            pos[i] = bounds;
        } else if (pos[i] < -bounds && vel[i] < 0) {
            vel[i] *= -REBOUND;
            pos[i] = -bounds;
        }
        pos[i] += timeStep * vel[i];
    }
}

void Particle::gravitate(Particle& o, double timeStep) {
    double dis = distance(o);
    double force = G * mass * o.mass / pow(dis, 2);


    double dir[D];
    for (int i = 0; i < D; ++i)
    {
        dir[i] = (pos[i] - o.pos[i]) / dis;
        vel[i] -= timeStep * force * dir[i] / mass;
        o.vel[i] += timeStep * force * dir[i] / o.mass;
    }
}

double Particle::distance(const Particle& o) const {
    double sum = 0.000001;
    for (int i = 0; i < D; ++i)
    {
        sum += pow(pos[i] - o.pos[i], 2);
    }
    return sqrt(sum);
}

void Particle::setColor(float r, float g, float b) {
    color[0] = r;
    color[1] = g;
    color[2] = b;
}

#endif