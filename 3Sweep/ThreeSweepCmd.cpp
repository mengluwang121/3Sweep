#include "ThreeSweepCmd.h"


const char *pointFlag = "-p", *pointLongFlag = "-point";
const char *modeFlag = "-m", *modeLongFlag = "-mode";
const char *nCurvesFlag = "-ncv", *nCurvesLongFlag = "-ncurves";

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
	syntax.addFlag(nCurvesFlag, nCurvesLongFlag, MSyntax::kDouble);
	syntax.addFlag(pointFlag, pointLongFlag, MSyntax::kDouble, MSyntax::kDouble, MSyntax::kDouble);

	syntax.makeFlagMultiUse(pointFlag);

	return syntax;
}


MStatus ThreeSweepCmd::doIt(const MArgList& args)
{
	MString info = "";
	
	MStatus* status;

	int mode = 0;
	int nCurves = 0;

	MArgList pointList;
	std::vector<vec3> points;
	
	MArgDatabase argData(newSyntax(), args);

	if (argData.isFlagSet(modeFlag))
		argData.getFlagArgument(modeFlag, 0, mode);

	if (argData.isFlagSet(nCurvesFlag))
		argData.getFlagArgument(nCurvesFlag, 0, nCurves);

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

	if (!manager) manager = new Manager();

	
	if (manager->number_of_strokes % 3 == 0) manager->end();
	manager->number_of_strokes = nCurves;
	//more control
	vec3 camera = vec3(0.0, 0.0, -1.0);
	for (int i = 0; i < numPoints; i++) {
		manager->update(points[i], camera, true);//?????????
	}

	Circle* result = (Circle*)(manager->curt_solution->curt);

	if (!result) {
		MGlobal::displayInfo("Circle is not computed");
		return MStatus::kSuccess;
	}

	if (nCurves % 3 == 2) {
		drawCircle(result, nCurves);
	}else if (nCurves % 3 == 0) {

		int index = (nCurves -1) / 3; //circle index

		//get right curve
		MString thirdStroke = "curve";
		thirdStroke += nCurves;

		//build circle:
		vec3 normal = result->getNormal();
		float radius = result->getRadius();
		MString cmd ="string $temp[] =`circle -c 0 0 0 -nr ";
		cmd += normal.x;
		cmd += " ";
		cmd += -normal.y;
		cmd += " ";
		cmd += normal.z;
		cmd += " -r ";
		cmd += radius;
		cmd += "`";
		MGlobal::displayInfo(cmd);
		MGlobal::executeCommand(cmd, true);
		//store the circle
		MString circle = "$myTempCircles[";
		circle += index;
		circle += "]";
		cmd = circle + "=$temp[0];";
		MGlobal::displayInfo(cmd);
		MGlobal::executeCommand(cmd, true);

		//extrude
		cmd = "extrude -ch true -rn false -po 1 -et 2 -ucp 1 -fpt 1 -upn 1 -rotation 0 -scale 1 -rsp 1 "+circle+" "+ thirdStroke+";";
		MGlobal::displayInfo(cmd);
		MGlobal::executeCommand(cmd, true);
	}

	return MStatus::kSuccess;
}

void ThreeSweepCmd::drawCircle(Circle* result, int numStrokes) {
	MString origin = "";
	origin += result->getOrigin().x;
	origin += " ";
	origin += result->getOrigin().y;
	origin += " ";
	origin += result->getOrigin().z;

	MString radius = "";
	radius += result->getRadius();

	MString normal = "";
	normal += result->getNormal().x;
	normal += " ";
	normal += result->getNormal().y;
	normal += " ";
	normal += result->getNormal().z;

	MGlobal::displayInfo(origin);
	MGlobal::displayInfo(radius);
	MGlobal::displayInfo(normal);

	MString cmd = "global string $circleFullName[]; $circleFullName = `circle -nr " + normal + " -c " + origin + " -r " + radius + "`";
	MGlobal::displayInfo(cmd); 
	MGlobal::executeCommand(cmd, true);

	cmd = "$myCircles[";
	cmd += (numStrokes - 1) / 3;
	cmd += "] = $circleFullName[0]";
	MGlobal::displayInfo(cmd);
	MGlobal::executeCommand(cmd, true);
}