#include "RodConstraint.h"
#include <GL/glut.h>

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

    iVector posdif = m_p1->m_Position - m_p2->m_Position;

    return  (posdif[0] * posdif[0] + posdif[1] * posdif[1] - m_dist * m_dist);
}

float RodConstraint::calcCD() {

    iVector pDiff = (m_p1->m_Position - m_p2->m_Position) * 2;
    iVector vDiff = (m_p1->m_Velocity - m_p2->m_Velocity) * 2;

    return pDiff[0] * vDiff[0] + pDiff[1] * vDiff[1];
}

std::vector< iVector > RodConstraint::j() {

    std::vector <iVector> j;

    j.push_back((m_p1->m_Position - m_p2->m_Position) * 2);
    j.push_back((m_p2->m_Position - m_p1->m_Position) * 2);

    return j;
}

std::vector< iVector > RodConstraint::jD() {

    std::vector <iVector> jD;

    jD.push_back((m_p1->m_Velocity - m_p2->m_Velocity) * 2);
    jD.push_back((m_p2->m_Velocity - m_p1->m_Velocity) * 2);

    return jD;
}

std::vector< int > RodConstraint::getParticleIDs() {

    return m_ids;

}