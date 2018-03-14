// Created by Christopher Nugent
// 
// The System class is used to manage a system of Particles in
// an organized manner, and also provides other methods used
// in rendering and modifying the system.

#ifndef SYSTEM_H
#define SYSTEM_H

#include <list>
#include <iostream>
#include <vector>
#include <cfloat>
#include "Particle.h"

class System {
private:
    static const int N = 15;
    static double randomFloat();
    void randomizeParticle(Particle& p);
    
    std::vector<Particle> particles;
    Particle com;
    
    void updateCOMS();
    double bounds;

public:
    static constexpr double MAX_MASS = pow(10, 11);

    std::list<double> history;

    System();
    
    // Mutators
    void initParticles();
    void update(double);
    void addParticle();
    void removeParticle(int i=-1);
    void setBounds(double nBounds);

    // Accessors
    int size() const;
    double pos(int i, int d) const;
    double comPos(int d) const;
    double color(int i, int d) const;
    double mass(int i) const;
    double getBounds() const;
    int closestParticle(Particle p) const;
    int getByColor(const float color[]) const;
    void printPositions() const;
};

#endif