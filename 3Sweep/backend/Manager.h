#pragma once
#include "Geometry.h"
#include "Solution.h"

using namespace glm;
class Manager
{
public:
	Manager();
	~Manager();
	void count();
	void init(const vec3 & camera_direction, std::string filename);
	void update(const vec3 & point, bool is_line);
	void update_line(const vec3 & point);
	void update_curve(const vec3 & point);
	void end();
	Solution* curt_solution;
	int number_of_strokes;
private:
	//std::vector<Solution*> solutions;
};

