#include "CircularWireConstraint.h"
#include <GL/glut.h>

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

CircularWireConstraint::CircularWireConstraint(Particle *p, const Vec2f &center, const double radius) :
        m_p(p), m_center(center), m_radius(radius) { }

void CircularWireConstraint::draw() {
    draw_circle(m_center, m_radius);
}

int CircularWireConstraint::calcC(){
    Vec2f posdif = (m_p->m_Position - m_center);
    float C = (posdif[0] * posdif[0] + posdif[1] * posdif[1] - m_radius*m_radius);
    return C;
}

int CircularWireConstraint::calcCD(){
    Vec2f posdif = (m_p->m_Position - m_center) * 2;
    Vec2f veldif = (m_p->m_Velocity) * 2;
    float CDot = posdif[0] * veldif[0] + posdif[1] * veldif[1];
    return CDot;
}

std::vector<iVector> CircularWireConstraint::j(){
    //a vector, only 1 now, might implement multiple particles
    std::vector<Vec2f> J;
    Vec2f posdif = (m_p->m_Position - m_center);
    Vec2f veldif = (m_p->m_Velocity);
    J.push_back(Vec2f(2 * posdif[0], 2 * posdif[1]));
    return J;
}

std::vector<iVector> CircularWireConstraint::jD(){
    std::vector<Vec2f> JDot;
    Vec2f veldif = (m_p->m_Velocity);
    JDot.push_back(Vec2f(2 * veldif[0], 2 * veldif[1]));
    return JDot;
}

std::vector<int> CircularWireConstraint::getParticleIDs(){
    return std::vector<int>();
}