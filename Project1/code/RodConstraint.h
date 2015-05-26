#pragma once

#include "Particle.h"
#include "Constraint.h"

#define iVector Vec2f

class RodConstraint : public Constraint {
public:
    RodConstraint(Particle *p1, Particle *p2, double dist);

    void draw();

    int calcC();

    int calcCD();

    std::vector<iVector> j();
    std::vector<iVector> jD();
    std::vector<int> getParticleIDs();

private:

    Particle *const m_p1;
    Particle *const m_p2;
    double const m_dist;
};
