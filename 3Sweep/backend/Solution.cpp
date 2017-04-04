#include "Solution.h"

using namespace glm;

bool Solution::compute()
{
	// TODO
	// right now this is only for circle
	if (!curt) return compute_circle();
	return update_circle();
}

bool Solution::compute_circle()
{	
	// if curt exists, should update the circle from the init one. 
	if (curt) return update_circle();
	// this is to compute the init circle
	// check the size of stroke before computing
	if (input.size() < 4) return false;
	// input.size >= 3
	// get the first 3 points in the Stroke
	// the first and second determine the origin and radius
	// the all 3 determine the plane normal
	vec3 first = input.getPoint(0);
	vec3 second = input.getPoint(1);
	vec3 third = input.getPoint(3);
	// for the 3rd sweep's start point
	// previous_point = third;
	/**** compute the orignal point ****/
	// middle point of 1st and 2nd
	vec3 origin = 0.5f * (first + second);
	// for cross product 
	vec3 r1 = second - origin;
	/**** compute the radius of the circle ****/
	// half of the distance between 1st and 2nd
	float radius = 0.5f * sqrt(dot(second - first, second - first));
	/**** compute the normal vector ****/
	// 1. get the short projection of radius
	vec3 proj_short = third - origin;
	// 2. calculate the length of the other right-angle side
	float right_angle_side_length = radius * radius - dot(proj_short, proj_short);
	if (right_angle_side_length < 0) right_angle_side_length = 0.0f;
	else right_angle_side_length = sqrt(right_angle_side_length);
	// 3. compute the offset vector in the reverse_camera_direction
	vec3 offset = right_angle_side_length * camera_direction;
	// 4. compute the real-world-axis vector of the short side
	vec3 r2 = proj_short + offset;
	// 5. calibrate
	// NEED DISCUSSION: cailbration
	// 6. compute the normal vector by rot
	vec3 normal = cross(r1, r2);
	// set output instance
	curt = new Circle(origin, radius, normal);
	// push the previous circle to the history list
	history.push_back(curt);
	//TEST
	//test(proj_short);
	//cout << "radius: " << radius << endl;
	//cout << "length(proj_short): " << length(proj_short) << endl;

	// return the success flag
	return true;
}

bool Solution::update_circle()
{
	// TEST BASICS
	//return true;
	const float TH_DOT_ERR = 0.1f;
	/**** TODO: update circle ****/
	// things in the edge detection
	// if size < 4: means the init is not over
	if (input.size() < 4) return false;
	for (int i = 4; i < input.size(); i++) {
		// get the circle pointer of previous one
		Circle* pre_circle = (Circle*)curt;
		// get curt point
		vec3 curt_point = input.getPoint(i);
		vec3 pre_point = input.getPoint(i - 1);
		vec3 next_point = i == input.size() ? curt_point : input.getPoint(i + 1);
		// compute the new origin, normal; calculate the new radius
		// TODO:: compute origin with two edges
		vec3 origin = curt_point;
		// TODO:: curt - pre is not correct
		vec3 normal_2d = normalize(next_point - pre_point);
		vec3 perpend = normalize(cross(normal_2d, camera_direction));
		// at this time contour vector should have been inited
		// compute the radius by shooting ray at each contour point
		float max = 0.0f, min = 0.0f;
		for (vec3 cp : contours) {
			vec3 ray = normalize(cp - origin);
			if (abs(dot(ray, normal_2d)) < TH_DOT_ERR) {
				float projection = dot(cp, perpend);
				max = fmaxf(max, projection);
				min = fminf(min, projection);
			}
		}
		float radius = pre_circle->getRadius();
		if (max != 0.0f && min != 0.0f) radius = 0.5f * (max - min);
		else if (max != 0.0f) radius = max;
		else if (min != 0.0f) radius = -min;
		// else remain the radius

		// construct a new circle 
		// TODO:: current normal is just for 2d
		curt = new Circle(origin, radius, normal_2d);
		// push the previous circle to the history list
		history.push_back(curt);
	}	
	return true;
}

void Solution::add_point(const vec3 & point)
{
	input.push(point);
}

void Solution::set_camera_direction(const vec3 & cd)
{
	camera_direction = cd;
}

void Solution::test(const vec3 & input)
{
	test(input, "TEST");
}

void Solution::test(const vec3 & input, char * name)
{
	std::cout << name << ": " << input.x << ", " << input.y << ", " << input.z << std::endl;
}
