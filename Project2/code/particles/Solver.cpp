#include "Particle.h"
#include "Force.h"
#include "Constraint.h"
#include <gfx/vec2.h>
#include "ConstraintSolver.h"
#include "../VectorField.h"

#include <vector>

#define IX_DIM(i, j) ((i)+(gridsize+2)*(j))

extern int solver;


void Euler(std::vector< Particle * > pVector, std::vector< Force * > fVector, float dt, std::vector< Constraint * > constraints);

void Midpoint(std::vector< Particle * > pVector, std::vector< Force * > fVector, float dt, std::vector< Constraint * > constraints);

void RungeKutta(std::vector< Particle * > pVector, std::vector< Force * > fVector, float dt, std::vector< Constraint * > constraints, VectorField *VelocityField);

void CalcForces(std::vector< Particle * > pVector, std::vector< Force * > fVector, std::vector< Constraint * > constraints);

void FluidVelocity(std::vector< Particle * > pVector, VectorField *VelocityField);

#define DAMP 0.98f
#define RAND (((rand()%2000)/1000.f)-1.f)

void simulation_step(std::vector< Particle * > pVector, std::vector< Force * > fVector, float dt, std::vector< Constraint * > constraints, VectorField *VelocityField) {

/*     if (solver == 1)
 *         Euler(pVector, fVector, dt, constraints);
 *     if (solver == 2)
 *         Midpoint(pVector, fVector, dt, constraints);
 *     if (solver == 3)
 */
	/* Always use RK4, since it's the most stable */
        RungeKutta(pVector, fVector, dt, constraints, VelocityField);
}


void Euler(std::vector< Particle * > pVector, std::vector< Force * > fVector, float dt, std::vector< Constraint * > constraints) {
    CalcForces(pVector, fVector, constraints);

    for (int i = 0; i < pVector.size(); i++) {
        pVector[i]->m_Position += dt * pVector[i]->m_Velocity;
        pVector[i]->m_Velocity += dt * pVector[i]->m_ForceVector / pVector[i]->m_Mass;
//		pVector[i]->m_Velocity = DAMP*pVector[i]->m_Velocity + Vec2f(RAND,RAND) * 0.005;
    }
}

void Midpoint(std::vector< Particle * > pVector, std::vector< Force * > fVector, float dt, std::vector< Constraint * > constraints) {
    int psize = pVector.size();

    std::vector< Vec2f > old_pos;
    std::vector< Vec2f > old_vel;

    for (int i = 0; i < psize; i++) {
        old_pos.push_back(pVector[i]->m_Position);
        old_vel.push_back(pVector[i]->m_Velocity);
    }

    CalcForces(pVector, fVector, constraints);

    for (int i = 0; i < psize; i++) {
        pVector[i]->m_Position += 0.5 * dt * pVector[i]->m_Velocity;
        pVector[i]->m_Velocity += 0.5 * dt * pVector[i]->m_ForceVector / pVector[i]->m_Mass;
    }

    CalcForces(pVector, fVector, constraints);

    for (int i = 0; i < psize; i++) {
        pVector[i]->m_Position = old_pos[i] + dt * pVector[i]->m_Velocity;
        pVector[i]->m_Velocity = old_vel[i] + dt * pVector[i]->m_ForceVector / pVector[i]->m_Mass;
    }
}

void RungeKutta(std::vector< Particle * > pVector, std::vector< Force * > fVector, float dt, std::vector< Constraint * > constraints, VectorField *VelocityField) {
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

    CalcForces(pVector, fVector, constraints);
    for (int i = 0; i < psize; i++) {
        k1[i] = pVector[i]->m_ForceVector / pVector[i]->m_Mass;
        l1[i] = pVector[i]->m_Velocity;

        pVector[i]->m_Position = begin_pos[i] + 0.5 * dt * l1[i];
        pVector[i]->m_Velocity = begin_vel[i] + 0.5 * dt * k1[i];
    }

    CalcForces(pVector, fVector, constraints);
    for (int i = 0; i < psize; i++) {
        k2[i] = pVector[i]->m_ForceVector / pVector[i]->m_Mass;
        l2[i] = pVector[i]->m_Velocity;

        pVector[i]->m_Position = begin_pos[i] + 0.5 * dt * l2[i];
        pVector[i]->m_Velocity = begin_vel[i] + 0.5 * dt * k2[i];
    }

    CalcForces(pVector, fVector, constraints);
    for (int i = 0; i < psize; i++) {
        k3[i] = pVector[i]->m_ForceVector / pVector[i]->m_Mass;
        l3[i] = pVector[i]->m_Velocity;

        pVector[i]->m_Position = begin_pos[i] + dt * l3[i];
        pVector[i]->m_Velocity = begin_vel[i] + dt * k3[i];
    }

    CalcForces(pVector, fVector, constraints);
    for (int i = 0; i < psize; i++) {
        k4[i] = pVector[i]->m_ForceVector / pVector[i]->m_Mass;
        l4[i] = pVector[i]->m_Velocity;

        pVector[i]->m_Position = begin_pos[i] + dt / 6.0 * (l1[i] + 2 * l2[i] + 2 * l3[i] + l4[i]);
        pVector[i]->m_Velocity = begin_vel[i] + dt / 6.0 * (k1[i] + 2 * k2[i] + 2 * k3[i] + k4[i]);
    }
	
    FluidVelocity(pVector, VelocityField);
}

void FluidVelocity(std::vector< Particle * > pVector, VectorField *VelocityField) 
{
	// Assume the particles are in a square grid
	int N_particles = pVector.size();
	int gridsize = VelocityField->m_NumCells;
	TVec2<int> mapped_position;

	for (int i = 0; i < N_particles; i++) {
		mapped_position[0] = (int) ((pVector[i]->m_Position[0]) * gridsize);
		mapped_position[1] = (int) ((pVector[i]->m_Position[1]) * gridsize);

		// Simple segfault prevention
		if (mapped_position[0] > 0 && mapped_position[1] > 0) 
			pVector[i]->m_Velocity = pVector[i]->m_Velocity + 1*VelocityField->m_Field[IX_DIM(mapped_position[0], mapped_position[1])];
	}
}

void CalcForces(std::vector< Particle * > pVector, std::vector< Force * > fVector, std::vector< Constraint * > constraints) {
    int fsize = fVector.size();
    int psize = pVector.size();

    for (int i = 0; i < psize; i++) {
        pVector[i]->m_ForceVector = Vec2f(0, 0);
    }

    for (int i = 0; i < fsize; i++) {
        fVector[i]->act();
    }

    for (int i = 0; i < psize; i++) {
	    if (pVector[i]->m_Fixed)
		    pVector[i]->m_ForceVector = Vec2f(0, 0);
    }

    solve(pVector, constraints, 60.0f, 5.0f);

}
