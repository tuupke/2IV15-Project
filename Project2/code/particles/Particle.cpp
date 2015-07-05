#include "Particle.h"
#include <GL/glut.h>

Particle::Particle(const Vec2f & ConstructPos, float Mass) :
	m_ConstructPos(ConstructPos), m_Position(Vec2f(0.0, 0.0)), m_Velocity(Vec2f(0.0, 0.0)), m_Mass(Mass), m_Fixed(false)
{
}

Particle::~Particle(void)
{
}

void Particle::reset()
{
	m_Position = m_ConstructPos;
	m_Velocity = Vec2f(0.0, 0.0);
}

void Particle::draw()
{
	const double h = 0.003;
	glColor3f(1.f, 1.f, 1.f); 
	glBegin(GL_QUADS);
	glVertex2f(m_Position[0]-h/2.0, m_Position[1]-h/2.0);
	glVertex2f(m_Position[0]+h/2.0, m_Position[1]-h/2.0);
	glVertex2f(m_Position[0]+h/2.0, m_Position[1]+h/2.0);
	glVertex2f(m_Position[0]-h/2.0, m_Position[1]+h/2.0);
	glEnd();

	// Draw the total force vector
	glColor3f(1.f, 0.f, 0.f); 
	glBegin(GL_LINES);
	glVertex2f(m_Position[0], m_Position[1]);
	glVertex2f(m_Position[0] + m_ForceVector[0], m_Position[1] + m_ForceVector[1]);
	glEnd();
}
