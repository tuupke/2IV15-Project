#include "Particle.h"
#include "Force.h"

#include <vector>

#define DAMP 0.98f
#define RAND (((rand()%2000)/1000.f)-1.f)
void simulation_step( std::vector<Particle*> pVector, std::vector<Force*> fVector, float dt )
{
	int ii, psize = pVector.size();
	int fsize = fVector.size();

	for(ii=0; ii<psize; ii++)
	{
		pVector[ii]->m_ForceVector = Vec2f(0,0);
	}

	for(ii=0; ii<fsize; ii++)
	{
		fVector[ii]->act();
	}
	
	for(ii=0; ii<psize; ii++)
	{
		pVector[ii]->m_Velocity += dt * pVector[ii]->m_ForceVector / pVector[ii]->m_Mass;
		pVector[ii]->m_Position += dt * pVector[ii]->m_Velocity;
		//pVector[ii]->m_Velocity = DAMP*pVector[ii]->m_Velocity + Vec2f(RAND,RAND) * 0.005;
	}

}

