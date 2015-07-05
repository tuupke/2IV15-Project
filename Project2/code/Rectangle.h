#pragma once

#include "VectorField.h"
#include "RigidBody.h"
#include "gfx/vec2.h"
#include <vector>
#include <algorithm>

class Rectangle : public RigidBody {
public :
    Rectangle(const Vec2f &c, float i, float w, float h);

    void draw();

    void act(VectorField *oldField, VectorField *newField);

    void emptyBody(VectorField *oldField, VectorField *newField);

    void reset();

private:

    Vec2f center;
    float inertia;
    Vec2f Velocity;
    float width;
    float height;
    float angle;

    std::vector<int> edge;
    std::vector<int> inner;

};