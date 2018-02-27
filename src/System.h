#ifndef SYSTEM
#define SYSTEM

#include <list>
#include "Particle.cpp"

class System {
private:
    void updateCOMS();
    double randomFloat();

public:
    static constexpr double MAX_MASS = pow(10, 11);

    static const int N = 20;

    Particle particles[N];
    Particle coms[NUM_GROUPS];
    Particle com;

    double totalMass;
    double bounds;

    std::list<double> history;

    System();
    void initParticles();
    void update(double);
};

#endif