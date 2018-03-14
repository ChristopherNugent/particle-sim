#ifndef PARTICLE_H
#define PARTICLE_H
#include <cmath>
#include <stdlib.h>

class Particle {
private:
    static constexpr double REBOUND = 0.75;
    static constexpr double G = 6.674 * pow(10, -11);
    
public:
    static const int D = 3;

    double pos[D];
    double vel[D];
    double mass;
    float color[3];

    Particle();
    void update(double timeStep, double bounds);
    void gravitate(Particle& o, double timeStep);
    double distance(const Particle& o) const;
    void setColor(float r, float g, float b);
};

#endif