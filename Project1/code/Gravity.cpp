#include "Particle.h"
#include "Gravity.h"
#include <GL/glut.h>

Gravity::Gravity(std::vector<Particle*> *particles, double gravity) :
 pVector(particles), g(gravity) {}

void Gravity::act()
{
	for (int i = 0; i < pVector.size(); i++) {
		pVector[i].m_ForceVector += pVector[i].m_Mass * g;
	}
}

void Gravity::draw()
{

}
