#ifndef SYSTEM_H
#define SYSTEM_H

#include <list>
#include <iostream>
#include <vector>
#include "Particle.h"

class System {
private:
    void updateCOMS();
    double randomFloat();
    void randomizeParticle(Particle& p);
    static const int N = 15;

public:
    static constexpr double MAX_MASS = pow(10, 11);

    std::vector<Particle> particles;
    std::list<double> history;
    Particle com;

    double totalMass;
    double bounds;

    System();
    void initParticles();
    void update(double);
    void printPositions();
    int size();
};

#endif