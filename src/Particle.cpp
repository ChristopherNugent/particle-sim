#include <cmath>
#include <list>

// arbitrary
const double G = 6.674 * pow(10, -11);
const int D = 3;
const double BOUNDS = 300;
const double REBOUND = 0.75;

class Particle {
public:

    double pos[D];
    double vel[D];

    double mass;


    float color[3];

    Particle() {
        for (int i = 0; i < 3; ++i) {
            color[i] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        }

    };

    void update(double timeStep) {
        for (int i = 0; i < D; ++i)
        {
            /* code */

            if (pos[i] > BOUNDS && vel[i] > 0) {
                vel[i] *= -REBOUND;
                pos[i] = BOUNDS;
            } else if (pos[i] < -BOUNDS && vel[i] < 0) {
                vel[i] *= -REBOUND;
                pos[i] = -BOUNDS;
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
};