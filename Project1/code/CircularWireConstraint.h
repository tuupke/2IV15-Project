#pragma once

#include "Particle.h"
#include "Constraint.h"
#include <vector>

class CircularWireConstraint : public Constraint {
public:
    CircularWireConstraint(Particle *p, const Vec2f &center, const double radius, std::vector<int> ids);

    void draw();

    float calcC();

    float calcCD();

    std::vector<iVector> j();
    std::vector<iVector> jD();
    std::vector<int> getParticleIDs();

private:

    Particle *const m_p;
    Vec2f const m_center;
    double const m_radius;
    std::vector<int> m_ids;
};
