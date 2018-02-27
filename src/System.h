#ifndef SYSTEM_H
#define SYSTEM_H

#include <list>
#include <iostream>
#include "Particle.h"

class System {
private:
    void updateCOMS();
    double randomFloat();

public:
    static constexpr double MAX_MASS = pow(10, 11);
    static const int N = 15;

    Particle particles[N];
    Particle com;

    double totalMass;
    double bounds;

    std::list<double> history;

    System();
    void initParticles();
    void update(double);
    void printPositions();
};

#endif