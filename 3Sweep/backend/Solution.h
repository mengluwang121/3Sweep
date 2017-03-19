#pragma once
#include "Circle.h"
#include "Stroke.h"
//#include "Test.h"

using namespace glm;
class Solution
{
public:
	Solution() 
	{
		curt = nullptr;
	}
	Solution(const Stroke & stroke)
	{
		Solution(vec3(0.0, 0.0, -1.0), stroke);
	}
	Solution(const vec3 & cd, const Stroke & stroke) 
	{
		curt = nullptr;
		camera_direction = vec3(cd);
		input = Stroke(stroke);
	}
	~Solution()
	{
		if (curt) delete curt;
		for (auto pointer : history) delete pointer;
	}
	bool compute();
	bool compute_circle();
	bool update_circle();
	void add_point(const vec3 & point);
	void set_camera_direction(const vec3 & cd);
	void test(const vec3 & input);
	void test(const vec3 & input, char* name);
	Geometry* curt;
private:
	vec3 camera_direction;
	//vec3 previous_point;
	Stroke input;
	std::vector<Geometry*> history;	
};

