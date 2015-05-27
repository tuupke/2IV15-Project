#pragma once

#include "Particle.h"
#include "Constraint.h"
#include <vector>

#define iVector Vec2f

class RodConstraint : public Constraint {
public:
    RodConstraint(Particle *p1, Particle *p2, double dist, std::vector<int> ids);

    void draw();

    float calcC();

    float calcCD();

    std::vector<iVector> j();
    std::vector<iVector> jD();
    std::vector<int> getParticleIDs();

private:

    Particle *const m_p1;
    Particle *const m_p2;
    std::vector<int> m_ids;
    double const m_dist;
};
