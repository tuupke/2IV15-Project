#pragma once
#include <vector>

#define iVector Vec2f

class Constraint {
public:
    virtual ~Constraint() { }

    virtual void draw() = 0;

    virtual float calcC() = 0;

    virtual float calcCD() = 0;

    virtual std::vector<iVector> j() = 0;
    virtual std::vector<iVector> jD() = 0;
    virtual std::vector<int> getParticleIDs() = 0;
};
