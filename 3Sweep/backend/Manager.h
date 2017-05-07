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
	void merge_solution(Solution* s);
	void clear_update_list();
	bool merge_two_circles(Solution* a, Solution* b);
	Solution* curt_solution;
	int number_of_strokes;
	std::string path;
	std::vector<Solution*> solutions;
	std::vector<int> update_list;

	static double TH_NORMAL; // threshold of dot(n1, n2)
	static double TH_ORIGIN; // threshold of distance(o1, o2)
	static double TH_RADIUS;

};

