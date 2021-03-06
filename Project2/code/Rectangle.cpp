#include "Rectangle.h"
#include "gfx/vec2.h"
#include <GL/glut.h>
#include <math.h>

#define IX(i, j) ((i)+(n+2)*(j))

Rectangle::Rectangle(const Vec2f &c, float i, float w, float h) :
        center(c), inertia(i), width(w), height(h), Velocity(Vec2f(0.0f, 0.0f)) {
    angle = 0;
    rotDif = 0;
}

bool pnpoly(int nvert, Vec2f vertices[], float testx, float testy) {
    int i, j;
    bool c = false;
    for (i = 0, j = nvert - 1; i < nvert; j = i++) {
        if (((vertices[i][1] > testy) != (vertices[j][1] > testy)) && 
	     (testx < ((vertices[j][0] - vertices[i][0]) * (testy - vertices[i][1]) / (vertices[j][1] - vertices[i][1]) + vertices[i][0])))
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
            float iI = (float)	    i  / (float) n;
            float jI = (float) (n - j) / (float) n;

            if (pnpoly(polyNum, vertices, iI, jI)) {

                for (int h = -1; h <= 1; h++) {
                    for (int v = -1; v <= 1; v++) {
                        if (h == 0 && v == 0) {
                            continue;
                        }

                        int vv = i + v;
                        int hh = j + h;

                        float vI = (float) vv / n;
                        float hI = float(n - hh) / n;

                        int index = IX(vv, hh);
                        if (!pnpoly(polyNum, vertices, vI, hI)) {
//                            std::cout << "Adding to aggregate " << newField->m_Field[index] << "\n";
                            aggregate += Velocity - newField->m_Field[index];
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
        aggregate[1] *= -1;
        aggregate /= width * height * n * n;
        Vec2f diff =  (aggregate-Velocity);
        Velocity += diff * inertia;
        center -= Velocity;
        float length = norm(aggregate);
        //2*M_PI-
        Vec2f aangle = Vec2f(cos(angle), sin(angle));
        unitize(aggregate);
        unitize(aangle);

        float inp = acos(aggregate[0] * aangle[0] + aggregate[1] * aangle[1]);

        rotDif = inp * length;//angle - aggregateAngle;
        //std::cout << "Current angle " << (angle / (2* M_PI)) << " aggregateAngle " << (aggregateAngle  / (2* M_PI))<< " rotDif " << (rotDif  / (2* M_PI))<< "\n"; 
        //std::cout << "Aggregate " << aggregate << " rotDiff " << rotDif << " Velocity " << Velocity << " length " << norm(Velocity) << " diff " << diff << " inertia " << inertia << "\n";

        angle += rotDif;
//        angle = fmod(angle, 2*M_PI);
    }
}

void Rectangle::reset(){
    Velocity = Vec2f(0.0f, 0.0f);
    angle = 0;
    rotDif = 0;
    center = Vec2f(0.5f, 0.5f);
}

void Rectangle::emptyBody(VectorField *newField, VectorField *oldField) {
    for(int i = 0; i < inner.size(); i++){
        newField->m_Field[inner[i]] = Vec2f(0.0f, 0.0f);
    }

    int centerXCell = (float) center[0] / 64;
    int centerYCell = float(64 - center[1]) / 64;

    int n = newField->m_NumCells;

    for(int i = 1; i < edge.size()-1; i++){
        int vectX = edge[i] % n - centerXCell;
        int vectY = edge[i] / n - centerYCell;

        int length = 1;//(sqrt(vectX * vectX + vectY * vectY)-35)/70+35;

        Vec2f perp = Vec2f(-vectY, vectX);

        unitize(perp);

        newField->m_Field[edge[i]] += (Velocity - perp * rotDif) * inertia;
//        newField->m_Field[edge[i]] *= 1.001f;

        // v = P2 - P1
        // P3 = (-v.y, v.x) / Sqrt(v.x^2 + v.y^2) * h
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

    glBegin(GL_LINES);
    glColor3f(1.0, 0.0, 0.0);

    glVertex2f(center[0], 1 - center[1]);

    glVertex2f(center[0] + 20*Velocity[0], 1 - center[1] - 20*Velocity[1]);
    
    glEnd();
}
