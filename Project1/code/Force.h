#pragma once

class Force
{
public:
	virtual ~Force(){}
	virtual void act()  = 0;
	virtual void draw() = 0;
};

