#pragma once
#include "Geometry.h"
#include <iostream>
using namespace glm;
class Square :
	public Geometry
{
public:
	Square();
	Square(const vec3 & original_point, double length_distance, const vec3 & normal_vector);
	~Square();
	void setOrigin(vec3 original_point);
	void setLength(double radius_distance);
	void setNormal(vec3 normal_vector);
	const vec3 getOrigin();
	const double getLength();
	const vec3 getNormal();
private:
	// original point
	vec3 origin;
	// the radius
	double length;
	// the normal of the circle plane
	vec3 normal;
};

