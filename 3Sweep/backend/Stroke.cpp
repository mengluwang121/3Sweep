#include "Stroke.h"

using namespace glm;

Stroke::Stroke()
{
}


Stroke::~Stroke()
{
}

vec3 Stroke::getPoint(int index)
{
	return point_list[index];
}

void Stroke::push(const vec3 & point)
{
	point_list.push_back(point);
}

void Stroke::pop()
{
	point_list.pop_back();
}

int Stroke::size()
{
	return point_list.size();
}
