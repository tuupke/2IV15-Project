#pragma once

#include "RigidBody.h"
#include <gfx/vec2.h>
#include <vector>

extern bool VorticityConfinement;

class RigidBody;

class VectorField {
public:
    VectorField(int a_NumCells, float a_Viscosity, float a_Dt);

    VectorField(VectorField *CopyField);

    virtual ~VectorField(void);

    void TimeStep(VectorField *a_SrcField, VectorField *VelocityField, std::vector< RigidBody * > & bodies);

    void AddField(VectorField *a_SrcField);

    Vec2f &operator[](int i) { return m_Field[i]; }

    int m_NumCells;
    Vec2f *m_Field;
    float m_Viscosity;
    float m_Dt;
};
