#pragma once

#include "Particle.h"
#include "Constraint.h"

class CircularWireConstraint : public Constraint {
public:
    CircularWireConstraint(Particle *p, const Vec2f &center, const double radius);

    void draw();

    int calcC();

    int calcCD();

    std::vector<iVector> j();
    std::vector<iVector> jD();
    std::vector<int> getParticleIDs();

//    std::vector <Vec2f> getJacobianPart();

private:

    Particle *const m_p;
    Vec2f const m_center;
    double const m_radius;
};
