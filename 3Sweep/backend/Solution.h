#pragma once
#include "Circle.h"
#include "Square.h"
#include "Stroke.h"
//#include "Test.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace glm;
class Solution
{
public:	
	enum Shape { NOTHING, CIRCLE, SQUARE };
	Solution() 
	{
		curt = nullptr;
	}
	Solution(const Stroke & stroke)
	{
		Solution(vec3(0.0f, -1.0f, 0.0f), stroke);
	}
	Solution(const vec3 & cd, const Stroke & stroke) 
	{
		curt = nullptr;
		camera_direction = vec3(cd);
		input = Stroke(stroke);
		shape = NOTHING; // as default NOTHING
	}
	~Solution()
	{
		for (auto pointer : history) delete pointer;
	}
	bool compute();
	bool compute_square();
	bool update_square();
	bool compute_circle();
	bool update_circle();
	void add_point(const vec3 & point);
	void set_camera_direction(const vec3 & cd);
	void test(const vec3 & input);
	void test(const vec3 & input, char* name);
	void set_contours(std::string filename);
	Geometry* curt;
	std::vector<Geometry*> history;
	Stroke input;
	std::vector<vec3> contours;	
	Shape shape;
private:
	vec3 camera_direction;
};

