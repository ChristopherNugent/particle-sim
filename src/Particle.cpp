#pragma once
#include <cmath>

// arbitrary

class Particle {
public:

    static const int D = 3;
    static constexpr double G = 6.674 * pow(10, -11);
    static constexpr double REBOUND = 0.75;
    double pos[D];
    double vel[D];

    double mass;


    float color[3];

    Particle() {
        for (int d = 0; d < D; ++d) {
            pos[d] = 0;
            vel[d] = 0;
        }
        mass = 0;
        for (int i = 0; i < 3; ++i) {
            color[i] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        }

    };

    void update(double timeStep, double bounds) {
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

    void gravitate(Particle& o, double timeStep) {
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

    double distance(const Particle& o) {
        double sum = 0.000001;
        for (int i = 0; i < D; ++i)
        {
            sum += pow(pos[i] - o.pos[i], 2);
        }
        return sqrt(sum);
    }

    void setColor(float r, float g, float b) {
        color[0] = r;
        color[1] = g;
        color[2] = b;
    }
};