#include "RodConstraint.h"
#include <GL/glut.h>

#define iVector Vec2f

RodConstraint::RodConstraint(Particle *p1, Particle *p2, double dist, std::vector<int> ids) :
        m_p1(p1), m_p2(p2), m_dist(dist), m_ids(ids) { }

void RodConstraint::draw() {
    glBegin(GL_LINES);
    glColor3f(0.8, 0.7, 0.6);
    glVertex2f(m_p1->m_Position[0], m_p1->m_Position[1]);
    glColor3f(0.8, 0.7, 0.6);
    glVertex2f(m_p2->m_Position[0], m_p2->m_Position[1]);
    glEnd();
}

float RodConstraint::calcC() {
    Vec2f posdif = m_p1->m_Position - m_p2->m_Position;
    float C = (posdif[0] * posdif[0] + posdif[1] * posdif[1] - m_dist * m_dist);
    return C;
}

float RodConstraint::calcCD() {
    Vec2f posdif = (m_p1->m_Position - m_p2->m_Position) * 2;
    Vec2f veldif = (m_p1->m_Velocity - m_p2->m_Velocity) * 2;
    return posdif[0] * veldif[0] + posdif[1] * veldif[1];
//    float CDot = vecDotNew(2 * posdif, 2 * veldif);
//    return CDot;
}

std::vector< iVector > RodConstraint::j() {
    std::vector <iVector> J;
    J.push_back(2.0 * (m_p1->m_Position - m_p2->m_Position));
    J.push_back(2.0 * (m_p2->m_Position - m_p1->m_Position));
    return J;
}

std::vector< iVector > RodConstraint::jD() {
    std::vector <iVector> JDot;
    JDot.push_back(2.0 * (m_p1->m_Velocity - m_p2->m_Velocity));
    JDot.push_back(2.0 * (m_p2->m_Velocity - m_p1->m_Velocity));
    return JDot;
}

std::vector< int > RodConstraint::getParticleIDs() {
    return m_ids;
}