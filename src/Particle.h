#ifndef PARTICLE_H
#define PARTICLE_H
#include <cmath>
#include <stdlib.h>

class Particle {
public:
    static const int D = 3;
    static constexpr double G = 6.674 * pow(10, -11);
    static constexpr double REBOUND = 0.75;

    double pos[D];
    double vel[D];
    double mass;
    float color[3];

    Particle();
    void update(double timeStep, double bounds);
    void gravitate(Particle& o, double timeStep);
    double distance(const Particle& o);
    void setColor(float r, float g, float b);
};

#endif