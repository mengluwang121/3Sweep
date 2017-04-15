#pragma once
#define isnan _isnan
#include "glm\glm.hpp"
#include "glm\vector_relational.hpp"
#include "glm\vec3.hpp"
#include <vector>
#include <iostream>
using namespace glm;
class Geometry
{
public:
	Geometry();
	virtual ~Geometry();
	virtual void setOrigin(vec3 original_point) {};
	virtual void setRadius(double radius_distance) {};
	virtual void setNormal(vec3 normal_vector) {};
	virtual const vec3 getOrigin() { return vec3(); };
	virtual const double getRadius() { return 0; };
	virtual const vec3 getNormal() { return vec3(); };
	virtual void setLength(double radius_distance) {};
	virtual const double getLength() { return 0; };
};

