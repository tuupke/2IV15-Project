#include "AngularForce.h"
#include <GL/glut.h>
#include <math.h>
#include <gfx/vec2.h>

AngularForce::AngularForce(Particle *p1, Particle *p2, Particle *p3, double angle, double ks, double kd) :
        m_p1(p1), m_p2(p2), m_p3(p3), m_angle(angle), m_ks(ks), m_kd(kd) { }

void AngularForce::draw() {
    glBegin(GL_LINES);
    glColor3f(0.6, 0.7, 0.8);
    glVertex2f(m_p1->m_Position[0], m_p1->m_Position[1]);
    glColor3f(0.6, 0.7, 0.8);
    glVertex2f(m_p2->m_Position[0], m_p2->m_Position[1]);
    glColor3f(0.6, 0.7, 0.8);
    glVertex2f(m_p3->m_Position[0], m_p3->m_Position[1]);
    glEnd();
}

void AngularForce::act() {

    Vec2f G = m_p1->m_Position - m_p3->m_Position;
    Vec2f H = m_p2->m_Position - m_p3->m_Position;

    unitize(G);
    unitize(H);

    float D = G*H;
    float arccosD = acos(D);
    float derivD = -1 / sqrt(1 - D * D);

    Vec2f result = (-m_ks * arccosD - m_kd * derivD) * derivD;
    m_p1->m_ForceVector += result;
    m_p2->m_ForceVector -= result;
}
