#include "CircularWireConstraint.h"
#include <GL/glut.h>

#define iVector Vec2f
#define PI 3.1415926535897932384626433832795

static void draw_circle(const Vec2f &vect, float radius) {
    glBegin(GL_LINE_LOOP);
    glColor3f(0.0, 1.0, 0.0);
    for (int i = 0; i < 360; i = i + 18) {
        float degInRad = i * PI / 180;
        glVertex2f(vect[0] + cos(degInRad) * radius, vect[1] + sin(degInRad) * radius);
    }
    glEnd();
}

CircularWireConstraint::CircularWireConstraint(Particle *p, const Vec2f &center, const double radius,
                                               std::vector< int > ids) :
        m_p(p), m_center(center), m_radius(radius), m_ids(ids) { }

void CircularWireConstraint::draw() {
    draw_circle(m_center, m_radius);
}

float CircularWireConstraint::calcC() {
    iVector diff = (m_p->m_Position - m_center);
    float C = (diff[0] * diff[0] + diff[1] * diff[1] - m_radius * m_radius);
    return C;
}

float CircularWireConstraint::calcCD() {
    iVector pDiff = (m_p->m_Position - m_center) * 2;
    iVector vDiff = (m_p->m_Velocity) * 2;
    float CDot = pDiff[0] * vDiff[0] + pDiff[1] * vDiff[1];
    return CDot;
}

std::vector< iVector > CircularWireConstraint::j() {
    std::vector< iVector > J;
    iVector pDiff = (m_p->m_Position - m_center) * 2;
    J.push_back(pDiff);
    return J;
}

std::vector< iVector > CircularWireConstraint::jD() {
    std::vector< iVector > JDot;
    iVector vDiff = (m_p->m_Velocity) * 2;
    JDot.push_back(vDiff);
    return JDot;
}

std::vector< int > CircularWireConstraint::getParticleIDs() {
    return m_ids;
}