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

    return diff[0] * diff[0] + diff[1] * diff[1] - m_radius * m_radius;
}

float CircularWireConstraint::calcCD() {

    iVector pDiff = (m_p->m_Position - m_center) * 2;
    iVector vDiff = (m_p->m_Velocity);

    return pDiff[0] * vDiff[0] + pDiff[1] * vDiff[1];
}

std::vector< iVector > CircularWireConstraint::j() {
    std::vector< iVector > j;

    j.push_back((m_p->m_Position - m_center) * 2);

    return j;
}

std::vector< iVector > CircularWireConstraint::jD() {
    std::vector< iVector > jD;

    jD.push_back(m_p->m_Velocity * 2);

    return jD;
}

std::vector< int > CircularWireConstraint::getParticleIDs() {
    return m_ids;
}