#include "Particle.h"
#include "Force.h"
#include <gfx/vec2.h>

#include <vector>

extern int solver;

void Euler(std::vector<Particle*> pVector, std::vector<Force*> fVector, float dt);
void Midpoint(std::vector<Particle*> pVector, std::vector<Force*> fVector, float dt);

void CalcForces(std::vector<Particle*> pVector, std::vector<Force*> fVector);

#define DAMP 0.98f
#define RAND (((rand()%2000)/1000.f)-1.f)
void simulation_step( std::vector<Particle*> pVector, std::vector<Force*> fVector, float dt )
{
	if (solver == 1)
		Euler(pVector, fVector, dt);
	if (solver == 2)
		Midpoint(pVector, fVector, dt);
}


void Euler(std::vector<Particle*> pVector, std::vector<Force*> fVector, float dt)
{
	CalcForces(pVector, fVector);

	for(int i=0; i < pVector.size(); i++)
	{
		pVector[i]->m_Position += dt * pVector[i]->m_Velocity;
		pVector[i]->m_Velocity += dt * pVector[i]->m_ForceVector / pVector[i]->m_Mass;
//		pVector[i]->m_Velocity = DAMP*pVector[i]->m_Velocity + Vec2f(RAND,RAND) * 0.005;
	}
}

void Midpoint(std::vector<Particle*> pVector, std::vector<Force*> fVector, float dt)
{
	int psize = pVector.size();

	std::vector<Vec2f> old_pos;
	std::vector<Vec2f> old_vel;

	for(int i=0; i < psize; i++)
	{
		old_pos.push_back(pVector[i]->m_Position);
		old_vel.push_back(pVector[i]->m_Velocity);
	}

	CalcForces(pVector, fVector);

	for(int i=0; i < psize; i++)
	{
        	pVector[i]->m_Position += 0.5 * dt * pVector[i]->m_Velocity;
		pVector[i]->m_Velocity += 0.5 * dt * pVector[i]->m_ForceVector / pVector[i]->m_Mass;
	}

	CalcForces(pVector, fVector);

	for(int i=0; i < psize; i++)
	{
        	pVector[i]->m_Position = old_pos[i] + dt * pVector[i]->m_Velocity;
		pVector[i]->m_Velocity = old_vel[i] + dt * pVector[i]->m_ForceVector / pVector[i]->m_Mass;
	}
}

void CalcForces(std::vector<Particle*> pVector, std::vector<Force*> fVector)
{
	int fsize = fVector.size();
 	int psize = pVector.size();

	for(int i=0; i < psize; i++)
	{
		pVector[i]->m_ForceVector = Vec2f(0,0);
	}

	for(int i=0; i < fsize; i++)
	{
		fVector[i]->act();
	}
}
