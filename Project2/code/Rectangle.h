#pragma once

#include "VectorField.h"
#include "RigidBody.h"
#include "gfx/vec2.h"

class Rectangle : public RigidBody {
public :
    Rectangle(const Vec2f &c, float i, float w, float h);

    void draw();

    void act(VectorField *oldField, VectorField *newField);

private:

    Vec2f center;
    float inertia;
    Vec2f Velocity;
    float width;
    float height;
    float angle;

};