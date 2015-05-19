#pragma once

#include "Particle.h"
#include "Force.h"
#include <vector>

class Gravity : public Force 
{
 public:
  Gravity(std::vector<Particle*> *particles, double gravity);

  void draw();
  void act();

 private:

  std::vector<Particle*> *pVector;
  double g;
};
