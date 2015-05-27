#include "Particle.h"
#include "Force.h"
#include "Constraint.h"
#include <gfx/vec2.h>
#include "ConstraintSolver.h"

#include <vector>

extern int solver;


void Euler(std::vector< Particle * > pVector, std::vector< Force * > fVector, float dt);

void Midpoint(std::vector< Particle * > pVector, std::vector< Force * > fVector, float dt);

void RungeKutta(std::vector< Particle * > pVector, std::vector< Force * > fVector, float dt);

void CalcForces(std::vector< Particle * > pVector, std::vector< Force * > fVector);

#define DAMP 0.98f
#define RAND (((rand()%2000)/1000.f)-1.f)

void simulation_step(std::vector< Particle * > pVector, std::vector< Force * > fVector, float dt, std::vector< Constraint * > constraints) {
    solve(pVector, constraints, 60.0f, 5.0f);
    if (solver == 1)
        Euler(pVector, fVector, dt);
    if (solver == 2)
        Midpoint(pVector, fVector, dt);
    if (solver == 3)
        RungeKutta(pVector, fVector, dt);
}


void Euler(std::vector< Particle * > pVector, std::vector< Force * > fVector, float dt) {
    CalcForces(pVector, fVector);

    for (int i = 0; i < pVector.size(); i++) {
        pVector[i]->m_Position += dt * pVector[i]->m_Velocity;
        pVector[i]->m_Velocity += dt * pVector[i]->m_ForceVector / pVector[i]->m_Mass;
//		pVector[i]->m_Velocity = DAMP*pVector[i]->m_Velocity + Vec2f(RAND,RAND) * 0.005;
    }
}

void Midpoint(std::vector< Particle * > pVector, std::vector< Force * > fVector, float dt) {
    int psize = pVector.size();

    std::vector< Vec2f > old_pos;
    std::vector< Vec2f > old_vel;

    for (int i = 0; i < psize; i++) {
        old_pos.push_back(pVector[i]->m_Position);
        old_vel.push_back(pVector[i]->m_Velocity);
    }

    CalcForces(pVector, fVector);

    for (int i = 0; i < psize; i++) {
        pVector[i]->m_Position += 0.5 * dt * pVector[i]->m_Velocity;
        pVector[i]->m_Velocity += 0.5 * dt * pVector[i]->m_ForceVector / pVector[i]->m_Mass;
    }

    CalcForces(pVector, fVector);

    for (int i = 0; i < psize; i++) {
        pVector[i]->m_Position = old_pos[i] + dt * pVector[i]->m_Velocity;
        pVector[i]->m_Velocity = old_vel[i] + dt * pVector[i]->m_ForceVector / pVector[i]->m_Mass;
    }
}

void RungeKutta(std::vector< Particle * > pVector, std::vector< Force * > fVector, float dt) {
    int psize = pVector.size();

    std::vector< Vec2f > begin_pos;
    std::vector< Vec2f > begin_vel;
    std::vector< Vec2f > k1, k2, k3, k4;
    std::vector< Vec2f > l1, l2, l3, l4;

    for (int i = 0; i < psize; i++) {
        begin_pos.push_back(pVector[i]->m_Position);
        begin_vel.push_back(pVector[i]->m_Velocity);
        k1.push_back(Vec2f(0.0, 0.0));
        k2.push_back(Vec2f(0.0, 0.0));
        k3.push_back(Vec2f(0.0, 0.0));
        k4.push_back(Vec2f(0.0, 0.0));
        l1.push_back(Vec2f(0.0, 0.0));
        l2.push_back(Vec2f(0.0, 0.0));
        l3.push_back(Vec2f(0.0, 0.0));
        l4.push_back(Vec2f(0.0, 0.0));
    }

    CalcForces(pVector, fVector);
    for (int i = 0; i < psize; i++) {
        k1[i] = pVector[i]->m_ForceVector / pVector[i]->m_Mass;
        l1[i] = pVector[i]->m_Velocity;

        pVector[i]->m_Position = begin_pos[i] + 0.5 * dt * l1[i];
        pVector[i]->m_Velocity = begin_vel[i] + 0.5 * dt * k1[i];
    }

    CalcForces(pVector, fVector);
    for (int i = 0; i < psize; i++) {
        k2[i] = pVector[i]->m_ForceVector / pVector[i]->m_Mass;
        l2[i] = pVector[i]->m_Velocity;

        pVector[i]->m_Position = begin_pos[i] + 0.5 * dt * l2[i];
        pVector[i]->m_Velocity = begin_vel[i] + 0.5 * dt * k2[i];
    }

    CalcForces(pVector, fVector);
    for (int i = 0; i < psize; i++) {
        k3[i] = pVector[i]->m_ForceVector / pVector[i]->m_Mass;
        l3[i] = pVector[i]->m_Velocity;

        pVector[i]->m_Position = begin_pos[i] + dt * l3[i];
        pVector[i]->m_Velocity = begin_vel[i] + dt * k3[i];
    }

    CalcForces(pVector, fVector);
    for (int i = 0; i < psize; i++) {
        k4[i] = pVector[i]->m_ForceVector / pVector[i]->m_Mass;
        l4[i] = pVector[i]->m_Velocity;

        pVector[i]->m_Position = begin_pos[i] + dt / 6.0 * (l1[i] + 2 * l2[i] + 2 * l3[i] + l4[i]);
        pVector[i]->m_Velocity = begin_vel[i] + dt / 6.0 * (k1[i] + 2 * k2[i] + 2 * k3[i] + k4[i]);
    }
}

void CalcForces(std::vector< Particle * > pVector, std::vector< Force * > fVector) {
    int fsize = fVector.size();
    int psize = pVector.size();

    for (int i = 0; i < psize; i++) {
        pVector[i]->m_ForceVector = Vec2f(0, 0);
    }

    for (int i = 0; i < fsize; i++) {
        fVector[i]->act();
    }


}
