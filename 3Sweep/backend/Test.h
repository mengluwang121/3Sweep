#pragma once
#include "Circle.h"
class Test
{
public:
	Test() {
		pointer = new Circle();
		test = vec3(0, 0, 0);
	}
	~Test() {
		if (pointer) delete pointer;
	}

private:
	Geometry* pointer;
	vec3 test;
};

