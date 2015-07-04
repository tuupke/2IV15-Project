#include "MouseForce.h"
#include <GL/glut.h>
#include <math.h>
#include <gfx/vec2.h>

MouseForce::MouseForce(std::vector<Particle*> particles, double dist, double ks, double kd, double radius) :
	m_radius(radius), pVector(particles), m_dist(dist), m_ks(ks), m_kd(kd), enabled(false) {}

void MouseForce::draw()
{
	if (enabled) {
		glBegin( GL_LINES );
		for (int i = 0; i < pVector.size(); i++) {
			Vec2f diffPosition = pVector[i]->m_Position - mousePos;
			if (norm(diffPosition) > m_radius)
				continue;
			glColor3f(0.0, 0.0, 1.0);
			glVertex2f( pVector[i]->m_Position[0], pVector[i]->m_Position[1] );
			glColor3f(0.0, 0.0, 1.0);
			glVertex2f( mousePos[0], mousePos[1] );
		}
		glEnd();
	}
}

void MouseForce::act()
{
	if (enabled) {
		for (int i = 0; i < pVector.size(); i++) {
			Vec2f diffPosition = pVector[i]->m_Position - mousePos;
			if (norm(diffPosition) > m_radius)
				continue;

			Vec2f diffSpeed = pVector[i]->m_Velocity;

			float distance = norm(diffPosition);
			float dotProd = diffSpeed * diffPosition;

			Vec2f result = (m_ks * (distance - m_dist) + m_kd * (dotProd / distance) ) * (diffPosition / distance);

			pVector[i]->m_ForceVector -= result;
		}
	}
}

void MouseForce::enable(float x, float y)
{
	enabled = true;
	mousePos = Vec2f(x, y);
}

void MouseForce::disable()
{
	enabled = false;
}
