#pragma once

#include "Particle.h"
#include "Force.h"
#include <vector>

class Gravity : public Force 
{
 public:
  Gravity(Particle *p, double gravity);

  void draw();
  void act();

 private:

  Particle * const m_p;
  double g;
};
