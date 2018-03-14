#ifndef SYSTEM_H
#define SYSTEM_H

#include <list>
#include <iostream>
#include <vector>
#include "Particle.h"

class System {
private:
    static const int N = 15;
    std::vector<Particle> particles;
    Particle com;
    
    void updateCOMS();
    double randomFloat();
    void randomizeParticle(Particle& p);
    double bounds;

public:
    static constexpr double MAX_MASS = pow(10, 11);

    std::list<double> history;

    System();
    void printPositions();
    
    // Mutators
    void initParticles();
    void update(double);
    void addParticle();
    void removeParticle(int i=-1);
    void setBounds(double nBounds);

    // Accessors
    int size() const;
    double pos(int i, int d) const;
    double color(int i, int d) const;
    double mass(int i) const;
    double comPos(int d) const;
    double getBounds();
};

#endif