#include "ThreeSweepCmd.h"


const char *pointFlag = "-p", *pointLongFlag = "-point";
const char *modeFlag = "-m", *modeLongFlag = "-mode";

Manager* ThreeSweepCmd::manager = nullptr;

ThreeSweepCmd::ThreeSweepCmd() : MPxCommand()
{
	//manager = nullptr;
}

ThreeSweepCmd::~ThreeSweepCmd()
{
	 //if (manager) delete manager;
}

MSyntax ThreeSweepCmd::newSyntax()
{
	MSyntax syntax;

	syntax.addFlag(modeFlag, modeLongFlag, MSyntax::kDouble);
	syntax.addFlag(pointFlag, pointLongFlag, MSyntax::kDouble, MSyntax::kDouble, MSyntax::kDouble);

	syntax.makeFlagMultiUse(pointFlag);

	return syntax;
}


MStatus ThreeSweepCmd::doIt(const MArgList& args)
{
	MString info = "";
	
	MStatus* status;

	int mode = 0;

	MArgList pointList;
	std::vector<vec3> points;
	
	MArgDatabase argData(newSyntax(), args);

	if (argData.isFlagSet(modeFlag))
		argData.getFlagArgument(modeFlag, 0, mode);

	info = mode;
	MGlobal::displayInfo(info);

	int numPoints = 0;
	
	if (argData.isFlagSet(pointFlag)) {
		numPoints = argData.numberOfFlagUses(pointFlag);
		
		for (int i = 0; i < numPoints; i++) {
			unsigned index = 0;
			argData.getFlagArgumentList(pointFlag, i, pointList);
			MPoint point = pointList.asPoint(index);
			info = point.x;
			info += point.y;
			info += point.z;
			MGlobal::displayInfo("add point: " + info);
			vec3 to_add(point.x, point.y, point.z);
			points.push_back(to_add);
		}
	}

	//call core function
	// Test test;

	if (!manager) manager = new Manager();
	vec3 camera = vec3(0.0, 0.0, -1.0);
	for (int i = 0; i < numPoints; i++) {
		manager->update(points[i], camera, true);
	}
	Circle* result = (Circle*)(manager->curt_solution->curt);
	if (!result) {
		MGlobal::displayInfo("Circle is not computed");
		return MStatus::kSuccess;
	}
	MString origin = "origin: ";
	origin += result->getOrigin().x;
	origin += ", ";
	origin += result->getOrigin().y;
	origin += ", ";
	origin += result->getOrigin().z;
	origin += "\\n";
	MString radius = "radius: ";
	radius += result->getRadius();
	radius += "\\n";
	MString normal = "normal: ";
	normal += result->getNormal().x;
	normal += ", ";
	normal += result->getNormal().y;
	normal += ", ";
	normal += result->getNormal().z;
	normal += "\\n";
	MGlobal::displayInfo(origin);
	MGlobal::displayInfo(radius);
	MGlobal::displayInfo(normal);
	// cout << "origin: " << result->getOrigin().x << ", " << result->getOrigin().y << ", " << result->getOrigin().z << endl;
	// cout << "radius: " << result->getRadius() << endl;
	// cout << "normal: " << result->getNormal().x << ", " << result->getNormal().y << ", " << result->getNormal().z << endl;



	return MStatus::kSuccess;
}