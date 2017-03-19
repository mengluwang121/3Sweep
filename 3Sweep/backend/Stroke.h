#pragma once
#include "Geometry.h"

using namespace glm;
class Stroke
{
public:
	Stroke();
	~Stroke();
	vec3 getPoint(int index);
	void push(const vec3 & point);
	void pop();
	int size();
private:
	std::vector<vec3> point_list;
};

