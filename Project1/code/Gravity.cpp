#include "Particle.h"
#include "Gravity.h"
#include <GL/glut.h>

Gravity::Gravity(Particle *p, Vec2f gravity) :
 m_p(p), g(gravity) {}

void Gravity::act()
{
	m_p->m_ForceVector += g * m_p->m_Mass;
}

void Gravity::draw()
{

}
