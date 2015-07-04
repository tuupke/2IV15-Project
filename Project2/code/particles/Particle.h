#pragma once

#include <gfx/vec2.h>

class Particle
{
public:

	Particle(const Vec2f & ConstructPos, float Mass);
	virtual ~Particle(void);

	void reset();
	void draw();

	Vec2f m_ConstructPos;
	Vec2f m_Position;
	Vec2f m_Velocity;
	Vec2f m_ForceVector;
	float m_Mass;
	bool m_Fixed;
};

