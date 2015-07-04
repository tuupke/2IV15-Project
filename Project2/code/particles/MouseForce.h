#pragma once

#include "Particle.h"
#include "Force.h"
#include <vector>

class MouseForce : public Force 
{
public:
	MouseForce(std::vector<Particle*> particles, double dist, double ks, double kd, double radius);

	void draw();
	void act();
	void enable(float x, float y);
	void disable();
	double m_radius;

private:

	std::vector<Particle*> pVector;
	Vec2f mousePos;
	double const m_dist;     // rest length
	double const m_ks, m_kd; // spring strength constants
	bool enabled;
};
