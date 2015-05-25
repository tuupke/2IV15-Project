#include "AngularForce.h"
#include <GL/glut.h>
#include <cmath>
#include <gfx/vec2.h>
#include <cstdio>

AngularForce::AngularForce(Particle *p1, Particle *p2, Particle *p3, double angle, double ks, double kd) :
	m_p1(p1), m_p2(p2), m_p3(p3), m_angle(angle), m_ks(ks), m_kd(kd) { }

void AngularForce::draw()
{
	glBegin(GL_LINES);
	glColor3f(0.0, 1.0, 0.0);
	glVertex2f(m_p1->m_Position[0], m_p1->m_Position[1]);
	glColor3f(0.0, 1.0, 0.0);
	glVertex2f(m_p2->m_Position[0], m_p2->m_Position[1]);

	glColor3f(0.0, 1.0, 0.0);
	glVertex2f(m_p2->m_Position[0], m_p2->m_Position[1]);
	glColor3f(0.0, 1.0, 0.0);
	glVertex2f(m_p3->m_Position[0], m_p3->m_Position[1]);
	glEnd();
}

void AngularForce::act()
{
	float pi = 3.141592653;

	float angle;
	Vec2f G = m_p1->m_Position - m_p2->m_Position;
	Vec2f H = m_p3->m_Position - m_p2->m_Position;

	float len12 = sqrt(G[0]*G[0] + G[1]*G[1]);
	float len32 = sqrt(H[0]*H[0] + H[1]*H[1]);

	float D = (G/len12)*(H/len32);
	float arccosD;
	if (D > -1.0 && D < 1.0)
		arccosD = acos(D);
	else if (D >= 1.0)
		arccosD = 0;
	else if (D <= -1.0)
		arccosD = pi;
		
	// acos(d) only works for 0* < d < 180*, so check the sign of the determinant
	// to find whether the angle is greater or less than 180*
	float determinant = (G[0] * H[1] - H[0] * G[1]);
	if (determinant < 0)
		angle = m_angle - arccosD;
	else
		angle = arccosD - m_angle;

	std::cout << D << "\t|\t" << arccosD << "\t|\t" << angle << std::endl;

	Vec2f dvel12 = m_p1->m_Velocity - m_p2->m_Velocity;
	Vec2f dvel32 = m_p3->m_Velocity - m_p2->m_Velocity;

	m_p1->m_ForceVector[0] -= (m_ks * angle * G[1]/len12) + (m_kd * dvel12[0]);
	m_p1->m_ForceVector[1] += (m_ks * angle * G[0]/len12) - (m_kd * dvel12[1]);
	m_p3->m_ForceVector[0] += (m_ks * angle * H[1]/len32) - (m_kd * dvel32[0]);
	m_p3->m_ForceVector[1] -= (m_ks * angle * H[0]/len32) + (m_kd * dvel32[1]);
}
