#include "ThreeSweepCmd.h"
#include <maya/MSelectionList.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnNurbsCurve.h>

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
	int subdivisionsX = 20;

	MArgDatabase argData(newSyntax(), args);

	if (argData.isFlagSet(modeFlag))
		argData.getFlagArgument(modeFlag, 0, mode);

	if (argData.isFlagSet(nCurvesFlag))
		argData.getFlagArgument(nCurvesFlag, 0, nCurves);

	int numPoints = 0;
	MArgList pointList;
	std::vector<vec3> points;

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
	// update from the last point
	// hardcode file name
	if (manager->curt_solution == nullptr) manager->init(camera, "C:\\Users\\SIG\\Source\\Repos\\3Sweep2\\3Sweep\\Banana.txt");

	MString test = "TEST: ";
	test += (int)(manager->curt_solution->contours.size());
	MGlobal::displayInfo(test);

	for (int i = 0; i < numPoints; i++) {
		manager->update(points[i], true);
	}
	manager->curt_solution->compute();

	Circle* circle_plane = (Circle*)(manager->curt_solution->curt);

	if (!circle_plane) {
		MGlobal::displayInfo("Circle is not computed");
		return MStatus::kSuccess;
	}
	if (nCurves % 3 == 2) {
		MGlobal::displayInfo("Circle is computed");
		//drawCircle(circle_plane, nCurves);
		vec3 origin = circle_plane->getOrigin();
		float radius = circle_plane->getRadius();
		vec3 normal = circle_plane->getNormal();

		int index = (nCurves+1) / 3;//index start from 1
		drawInitialCylinder(radius, origin, normal, subdivisionsX, index);

	}else if (nCurves % 3 == 0) {

		MGlobal::displayInfo("Extruding");

		int index = nCurves/ 3; //circle index
		MString thirdStroke = "curve";
		thirdStroke += nCurves;//the third curve name

		//build temp circle
		Circle* pre_circle = (Circle*)(manager->curt_solution->history[0]);
		Circle* curt_circle = pre_circle;

		for (int i = 1; i < manager->curt_solution->history.size(); i++) {
			// Done:: change to list of circle
			curt_circle = (Circle*)(manager->curt_solution->history[i]);
			vec3 lastNormal = pre_circle->getNormal();

			float lastRadius = pre_circle->getRadius();
			float curRadius = curt_circle->getRadius();
			float scaleRatio = (curRadius/ lastRadius) * (curRadius / lastRadius);
			// TEST START
			MString radiusString = "Radius: ";
			radiusString += curRadius;
			radiusString += "; LastRadius: ";
			radiusString += lastRadius;
			MGlobal::displayInfo(radiusString);
			// TEST END
			vec3 start = pre_circle->getOrigin(); 
			vec3 end = curt_circle->getOrigin();
			// update pre_circle
			pre_circle = curt_circle;
			MString cylinderName = "Cylinder";
			cylinderName += index;
			int startIdx = subdivisionsX;
			int endIdx = subdivisionsX * 2-1;
			vec3 translateW = end - start;//word 
			//vec3 translate = -cross(vec3(1, 0, 0), translateW);
			vec3 direction = vec3(0, 0, 0);
			vec3 scale = vec3(scaleRatio, scaleRatio, scaleRatio);
			extrude(cylinderName, startIdx, endIdx, translateW, direction, scale);
	
			//build start circle and curveSegment
	/*		MString cmd ="circle -c 0 0 0 -nr ";
			cmd += lastNormal.x;
			cmd += " ";
			cmd += -lastNormal.y;
			cmd += " ";
			cmd += 0;
			cmd += " -r ";
			cmd += lastRadius;
			cmd += " -n startCircle;";

			cmd += "curve - d 1 -p ";
			cmd += start.x;
			cmd += " ";
			cmd += start.y;
			cmd += " ";
			cmd += start.z;
			cmd += " -p ";
			cmd += end.x;
			cmd += " ";
			cmd += end.y;
			cmd += " ";
			cmd += end.z;
			cmd += " -n curveSeg;";
			MGlobal::displayInfo(cmd);
			MGlobal::executeCommand(cmd, true);*/

			//extrude
			//cmd = "extrude -ch true -rn false -po 1 -et 2 -ucp 1 -fpt 1 -upn 1 -rotation 0 -scale ";
			//cmd += scale;
			//cmd += " -rsp 1 startCircle curveSeg";
			//MGlobal::displayInfo(cmd);
			//MGlobal::executeCommand(cmd, true);

			////reset
			//cmd = "delete startCircle; delete curveSeg;";
			//MGlobal::displayInfo(cmd);
			//MGlobal::executeCommand(cmd, true);
		}
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

	cmd = "$initialCircles[";
	cmd += (numStrokes - 1) / 3;
	cmd += "] = $circleFullName[0]";
	MGlobal::displayInfo(cmd);
	MGlobal::executeCommand(cmd, true);
}

void ThreeSweepCmd::drawInitialCylinder(float radius, vec3 origin, vec3 ax, int sx, int index) {
	MString cmd = "";
	cmd = "polyCylinder -h 0.001 -r ";
	cmd += radius;
	cmd += " -sx ";
	cmd += sx;
	cmd += " -sy 1 -sz 1 -rcp 0 -cuv 3 -ch 1 -ax ";
	cmd += ax.x;
	cmd += " ";
	cmd += ax.y;
	cmd += " "; 
	cmd += 0;
	cmd += " -n Cylinder";
	cmd += index;
	cmd += "; move -r ";
	cmd += origin.x;
	cmd += " ";
	cmd += origin.y;
	cmd += " ";
	cmd += origin.z;

	MGlobal::displayInfo(cmd);
	MGlobal::executeCommand(cmd, true);
}

void ThreeSweepCmd::extrude(MString objName, int startIdx, int endIdx, vec3 translate, vec3 direction, vec3 scale) {
	
	MString cmd = "PolyExtrude; polyExtrudeFacet -constructionHistory 1 -keepFacesTogether 1 -pvx ";
	cmd += "0 -pvy 1 -pvz 0 -divisions 1 -twist 0 -taper 1 -off 0 -thickness 0 -smoothingAngle 30 -t ";
	cmd += translate.x;
	cmd += " ";
	cmd += translate.y;
	cmd += " ";
	cmd += translate.z; 
	cmd += " -ls ";
	cmd += scale.x;
	cmd += " ";
	cmd += scale.y;
	cmd += " ";
	cmd += scale.z;
	cmd += " ";
	cmd += objName;
	cmd += ".f[";
	cmd += startIdx;
	cmd += ":";
	cmd += endIdx;
	cmd += "]";
	MGlobal::displayInfo(cmd);
	MGlobal::executeCommand(cmd, true);
}

std::vector<vec3> getSamplePoints(int nCurves) {
	//get sample points
	std::vector<vec3> pointsOnCurve;
	int nSample = 20;
	float step = 1.0 / 20;
	float pctParam = 0;

	MString thirdStrokeShape = "curveShape";
	thirdStrokeShape += nCurves;
	MGlobal::selectByName(thirdStrokeShape, MGlobal::kReplaceList);
	MSelectionList selected;
	MGlobal::getActiveSelectionList(selected);

	MStatus status;
	// returns the i'th selected dependency node
	MObject obj;
	selected.getDependNode(0, obj);
	if (obj.hasFn(MFn::kNurbsCurve)) {
		// Attach a function set to the selected object
		MFnNurbsCurve curveObj(obj);
		for (int i = 0; i < nSample; i++) {
			MPoint point;
			status = curveObj.getPointAtParam(pctParam, point);
			MString info;
			info = point.x;
			info += point.y;
			info += point.z;
			MGlobal::displayInfo(info);

			vec3 pointVec(point.x, point.y, point.z);
			pointsOnCurve.push_back(pointVec);

			pctParam += step;
		}
	}
	else {
		MGlobal::displayInfo("not curves!");
	}

	return pointsOnCurve;
}