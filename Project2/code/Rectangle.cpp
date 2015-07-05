#include "Rectangle.h"
#include "gfx/vec2.h"
#include <GL/glut.h>
#include <math.h>

#define IX(i, j) ((i)+(n+2)*(j))

Rectangle::Rectangle(const Vec2f &c, float i, float w, float h) :
        center(c), inertia(i), width(w), height(h), Velocity(Vec2f(0.0f, 0.0f)) {
    angle = 0;
}

bool pnpoly(int nvert, Vec2f vertices[], float testx, float testy) {
    int i, j;
    bool c = false;
    for (i = 0, j = nvert - 1; i < nvert; j = i++) {
        if (((vertices[i][1] > testy) != (vertices[j][1] > testy)) &&
            (testx < (vertices[j][0] - vertices[i][0]) * (testy - vertices[i][1]) / (vertices[j][1] - vertices[i][1]) +
                     vertices[i][0]))
            c = !c;
    }
    return c;
}

void Rectangle::act(VectorField *newField, VectorField *oldField) {


    // Set all velocity components to 0
    int n = oldField->m_NumCells;


    float startX = center[0] - width / 2;
    float startY = center[1] - height / 2;

    float endX = center[0] + width / 2;
    float endY = center[1] + height / 2;

    float difX = startX - endX;
    float difY = startY - endY;

    // Diagonal length from center to corner, equal for all corners
    float diagonalLength = sqrt(difX * difX + difY * difY) / 2;
    float innerAngle = difY == 0 ? M_PI / 4 : atan(difX / difY);

    Vec2f vertices[4] = {};

    // Top left corner
    float tlAngle = M_PI - innerAngle + angle;
    vertices[0] = Vec2f(center[0] + cos(tlAngle) * diagonalLength, center[1] + sin(tlAngle) * diagonalLength);

    // Top right corner
    float trAngle = innerAngle + angle;
    vertices[1] = Vec2f(center[0] + cos(trAngle) * diagonalLength, center[1] + sin(trAngle) * diagonalLength);
//    vertices[1] = Vec2f(center[0] + 0.0f, center[1] + 0.0f);

    // Bottom right corner
    float brAngle = -innerAngle + angle;
    vertices[2] = Vec2f(center[0] + cos(brAngle) * diagonalLength, center[1] + sin(brAngle) * diagonalLength);
//    vertices[2] = Vec2f(center[0] + 0.0f, center[1] + 0.0f);

    // Bottom left corner
    float blAngle = M_PI + innerAngle + angle;
    vertices[3] = Vec2f(center[0] + cos(blAngle) * diagonalLength, center[1] + sin(blAngle) * diagonalLength);
//    vertices[3] = Vec2f(center[0] + 0.0f, center[1] + 0.0f);

    int polyNum = 4;

    Vec2f aggregate = Vec2f(0.0f, 0.0f);

    inner.clear() ;// = new std::vector<int>();
    edge.clear();// = new std::vector<int>();

    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= n; j++) {
            float iI = (float) i / 64;
            float jI = float(64 - j) / 64;

            if (pnpoly(polyNum, vertices, iI, jI)) {

                for (int h = -1; h <= 1; h++) {
                    for (int v = -1; v <= 1; v++) {
                        if (h == 0 && v == 0) {
                            continue;
                        }

                        int vv = i + v;
                        int hh = j + h;

                        float vI = (float) vv / 64;
                        float hI = float(64 - hh) / 64;

                        int index = IX(vv, hh);
                        if (!pnpoly(polyNum, vertices, vI, hI)) {
                            aggregate += oldField->m_Field[index];
                            std::vector<int>::iterator it = std::find(edge.begin(), edge.end(), index);
                            if(it == edge.end())
                                edge.push_back(index);
                        } else {
                            std::vector<int>::iterator it = std::find(inner.begin(), inner.end(), index);
                            if(it == inner.end())
                                inner.push_back(index);
                        }
                    }
                }
                int index = IX(i, j);
                std::vector<int>::iterator iv = std::find(edge.begin(), edge.end(), index);
                std::vector<int>::iterator it = std::find(inner.begin(), inner.end(), index);
                if(it == inner.end() && iv == edge.end())
                    inner.push_back(index);
            }
        }
    }


    if (aggregate[0] != 0 || aggregate[1] != 0) {
        aggregate /= n * 6;
        Vec2f diff =  (Velocity - aggregate);
        center += diff;
        Velocity += diff * inertia * 0.1f;
        float aggregateAngle = atan(aggregate[0] / aggregate[1]) - angle;
//        std<<
        angle += aggregateAngle * inertia;
        angle = fmod(angle, M_PI);
    }
//
//    for(int i = 0; i < edge.size(); i++){
//        newField->m_Field[edge[i]] = -oldField->m_Field[edge[i]]*10000000000;// + Velocity * inertia;
//    }
}

void Rectangle::reset(){
    Velocity = Vec2f(0.0f, 0.0f);
    angle = 0;
    center = Vec2f(0.5f, 0.5f);
}

void Rectangle::emptyBody(VectorField *newField, VectorField *oldField) {
    for(int i = 0; i < inner.size(); i++){
        newField->m_Field[inner[i]] = Vec2f(0.0f, 0.0f);
    }

    for(int i = 0; i < edge.size(); i++){
//        std::cout << "Edge " << i << " has vector: " << newField->m_Field[edge[i]] << "\n";
        newField->m_Field[edge[i]] = Velocity;
    }
}

void Rectangle::draw() {
    extern bool drawLine;

    if (drawLine) {
        glBegin(GL_LINE_LOOP);
    } else {
        glBegin(GL_TRIANGLE_FAN);
    }

    glColor3f(0.0, 1.0, 1.0);

    float startX = center[0] - width / 2;
    float startY = -center[1] - height / 2;

    float endX = center[0] + width / 2;
    float endY = -center[1] + height / 2;

    float difX = startX - endX;
    float difY = startY - endY;

    angle *= -1;

    // Diagonal length from center to corner, equal for all corners
    float diagonalLength = sqrt(difX * difX + difY * difY) / 2;
    float innerAngle = difY == 0 ? M_PI / 4 : atan(difX / difY);

    // Top left corner
    float tlAngle = M_PI - innerAngle + angle;
    glVertex2f(center[0] + cos(tlAngle) * diagonalLength, 1 - center[1] + sin(tlAngle) * diagonalLength);

    // Top right corner
    float trAngle = innerAngle + angle;
    glVertex2f(center[0] + cos(trAngle) * diagonalLength, 1 - center[1] + sin(trAngle) * diagonalLength);

    // Bottom right corner
    float brAngle = -innerAngle + angle;
    glVertex2f(center[0] + cos(brAngle) * diagonalLength, 1 - center[1] + sin(brAngle) * diagonalLength);

    // Bottom left corner
    float blAngle = M_PI + innerAngle + angle;
    glVertex2f(center[0] + cos(blAngle) * diagonalLength, 1 - center[1] + sin(blAngle) * diagonalLength);

    angle *= -1;

    glEnd();
}