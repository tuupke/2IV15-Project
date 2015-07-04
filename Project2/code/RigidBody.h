#pragma once

#include "VectorField.h"

class VectorField;

class RigidBody {
public:
    virtual ~RigidBody() { }

    virtual void draw() = 0;

    virtual void act(VectorField *oldField, VectorField *newField) = 0;



};