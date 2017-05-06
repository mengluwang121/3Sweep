#include "Manager.h"



Manager::Manager()
{
	curt_solution = nullptr;
	number_of_strokes = 0;
	path = "";
}


Manager::~Manager()
{
	if (curt_solution) delete curt_solution;
	for (auto pointer : solutions)
	{
		delete pointer;
	}
}

void Manager::count()
{
	number_of_strokes++;
}

void Manager::init(const vec3 & camera_direction, std::string filename)
{
	if (curt_solution) return;
	Stroke stroke = Stroke();
	curt_solution = new Solution(camera_direction, stroke);
	curt_solution->set_contours(filename);
}

void Manager::update(const vec3 & point, bool is_line)
{
	if (is_line) update_line(point);
	else update_curve(point);
}

void Manager::update_line(const vec3 & point)
{
	// TODO 
	curt_solution->add_point(point);
}

void Manager::update_curve(const vec3 & point)
{
	// TODO
	curt_solution->add_point(point);
}

void Manager::end()
{
	solutions.push_back(curt_solution);
	curt_solution = nullptr;
}

void Manager::merge_solution(Solution * s)
{
	update_list.clear();
	for (int i = 0; i < solutions.size(); i++) {
		Solution* sol = solutions[i];
		if (merge_two_circles(s, sol)) {
			// update the circles in sol
			sol->update_circle();
			update_list.push_back(i);
		}
	}
	// finally update all the circles
	// this would be 
	// s->update_circle();
}

void Manager::clear_update_list()
{
	update_list.clear();
}

bool Manager::merge_two_circles(Solution * a, Solution * b)
{
	float TH_NORMAL = 0.1f; // threshold of dot(n1, n2)
	float TH_ORIGIN = 0.1f; // threshold of distance(o1, o2)
	Circle* c1 = (Circle*)(a->curt);
	Circle* c2 = (Circle*)(b->curt);
	vec3 n1 = c1->getNormal();
	vec3 n2 = c2->getNormal();
	vec3 o1 = c1->getOrigin();
	vec3 o2 = c2->getOrigin();
	if (1.0f - fabsf(dot(n1, n2)) < TH_NORMAL) {
		// align the normals 
		// n1, n2 in 180 degree
		if (dot(n1, n2) < 0.0f) {
			vec3 avg = normalize(0.5f * (n2 - n1));
			n2 = avg;
			n1 = -avg;
		}
		// n1, n2 in 0 degree
		else {
			vec3 avg = normalize(0.5f * (n1 + n2));
			n1 = avg;
			n2 = avg;
		}
		// set the new normal
		c1->setNormal(n1);
		c2->setNormal(n2);
		// merge the origins
		if (distance(o1, o2) < TH_ORIGIN) {
			vec3 avg = 0.5f * (o1 + o2);
			o1 = avg;
			o2 = avg;
			// set the new origin
			c1->setOrigin(o1);
			c2->setOrigin(o2);
		}
		// merged
		return true;
	}
	// nothing updated
	return false;

}

//int main() {
//	Manager manager = Manager();
//	vec3 camera = vec3(0.0, 0.0, -1.0);
//	manager.update(vec3(-2, 0, 0), camera, true);
//	manager.update(vec3(2, 0, 0), camera, true);
//	manager.update(vec3(0, -1, 0), camera, true);
//	manager.update(vec3(0, -2, 0), camera, false);
//	manager.update(vec3(0, -3, 0), camera, false);
//	Circle* result = (Circle*)(manager.curt_solution->curt);
//	cout << "origin: " << result->getOrigin().x << ", " << result->getOrigin().y << ", " << result->getOrigin().z << endl;
//	cout << "radius: " << result->getRadius() << endl;
//	cout << "normal: " << result->getNormal().x << ", " << result->getNormal().y << ", " << result->getNormal().z << endl;
//	return 0;
//}