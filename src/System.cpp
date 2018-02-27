#include "Particle.cpp"

class System {
public:
    static constexpr double MAX_MASS = pow(10, 11);
    static const int N = 10;
    static constexpr int NUM_GROUPS = sqrt(N) + 1;
    static constexpr int GROUP_SIZE = sqrt(N);

    Particle particles[N];
    Particle coms[NUM_GROUPS];
    Particle com;

    double totalMass = -1;

    std::list<double> history;

    System() {}

    void initParticles() {
        history = {};
        totalMass = 0;
        for (int i = 0; i < N; i++) {
            double mass = MAX_MASS * randomFloat();
            particles[i].mass = mass;
            totalMass += mass;
            for (int i = 0; i < Particle::D; ++i) {
                particles[i].pos[i] = 40 * randomFloat() - 20;
                particles[i].vel[i] = 10 * randomFloat() - 5;
            }
        }
    }

    void updateCOMS() {
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


    void update(double timeStep) {
        updateCOMS();
        for(int i = 0; i < N; i++) {
            for(int j = i + 1; j < N; j++) {
                particles[i].gravitate(particles[j], timeStep);
            }
            particles[i].update(timeStep);
        }
    }

    double randomFloat() {
        return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    }


};