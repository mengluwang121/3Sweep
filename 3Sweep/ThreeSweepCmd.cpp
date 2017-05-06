#include "ThreeSweepCmd.h"
#include <maya/MSelectionList.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnNurbsCurve.h>
#include <cstdlib>
#include <string>
#include <sstream>

const char *geometryNameFlag = "-gn", *geometryNameLongFlag = "-geoname";
const char *pathFlag = "-pth", *pathLongFlag = "-path";
const char *pointFlag = "-p", *pointLongFlag = "-point";
const char *modeFlag = "-m", *modeLongFlag = "-mode";
const char *nCurvesFlag = "-ncv", *nCurvesLongFlag = "-ncurves";
const char *recomputeFlag = "-rc", *recomputeLongFlag = "-recompute";

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

	syntax.addFlag(geometryNameFlag, geometryNameLongFlag, MSyntax::kString);
	syntax.addFlag(pathFlag, pathLongFlag, MSyntax::kString);
	syntax.addFlag(modeFlag, modeLongFlag, MSyntax::kDouble);
	syntax.addFlag(nCurvesFlag, nCurvesLongFlag, MSyntax::kDouble);
	syntax.addFlag(pointFlag, pointLongFlag, MSyntax::kDouble, MSyntax::kDouble, MSyntax::kDouble);
	syntax.addFlag(recomputeFlag, recomputeLongFlag, MSyntax::kBoolean);

	syntax.makeFlagMultiUse(pointFlag);

	return syntax;
}

MStatus ThreeSweepCmd::doIt(const MArgList& args)
{
	MString info = "";
	
	MStatus* status;

	if (!manager) manager = new Manager();

	MArgDatabase argData(newSyntax(), args);

	int mode = 0;
	int nCurves = 0;
	int subdivisionsX = 20;
	bool recompute = true;
	MString curGeometry = "";

	//preprocessing
	if (argData.isFlagSet(pathFlag)) {

		MString path = "";
		argData.getFlagArgument(pathFlag, 0, path);
		manager->path = path.asChar();
		
		MGlobal::displayInfo("Preprocessing: " + path);
		int retCode = preProcess(path);
		if (retCode == 0) {
			return MStatus::kSuccess;
		}
		else {
			MGlobal::displayInfo("initialize failure!");
			return MStatus::kFailure;
		}
	}

	//which stroke is being processed. 
	if (argData.isFlagSet(modeFlag))
		argData.getFlagArgument(modeFlag, 0, mode);

	if (argData.isFlagSet(nCurvesFlag))
		argData.getFlagArgument(nCurvesFlag, 0, nCurves);

	if (argData.isFlagSet(geometryNameFlag))
		argData.getFlagArgument(geometryNameFlag, 0, curGeometry);

	std::string curGeometryStr = curGeometry.asChar();
	std::string indexStr = curGeometryStr.substr(8, curGeometryStr.length());
	int index = std::stoi(indexStr);
	
	if (argData.isFlagSet(recomputeFlag))
		argData.getFlagArgument(recomputeFlag, 0, recompute);

	if (!recompute) {
		if (manager->solutions.size() < index) {
			MGlobal::displayError("Geometry information does not exist, please set recompute flag to true!");
			return MStatus::kFailure;
		}
		//update the scene with internal information;

		manager->curt_solution = manager->solutions[index];

		//update cur solution
		int shape = manager->curt_solution->shape;

		if (shape == Solution::Shape::CIRCLE) {
		
		Geometry* pre_plane = (manager->curt_solution->history[0]);
	 	Circle* circle_plane = (Circle*)pre_plane;
		vec3 origin = circle_plane->getOrigin();
		float radius = circle_plane->getRadius();
		vec3 normal = circle_plane->getNormal();

		MString circleName = curGeometry;
		circleName += "Circle0";
		drawCircle(origin, normal, radius, circleName);

		for (int i = 1; i < manager->curt_solution->history.size(); i++) {
			Geometry* curt_plane = (manager->curt_solution->history[i]);
			vec3 curOrigin = curt_plane->getOrigin();
			vec3 curNormal = curt_plane->getNormal();
			float curRadius = curt_plane->getRadius();
			
			MString curCircle = curGeometry;
			curCircle += "Circle";
			curCircle += i;
			//draw current circle;
			drawCircle(curOrigin, curNormal, curRadius, curCircle);

			vec3 preOrigin = pre_plane->getOrigin();
			vec3 preNormal = pre_plane->getNormal();
			float preRadius = pre_plane->getRadius();

			float scaleRatio = 0;
			scaleRatio = (curRadius / preRadius) * (curRadius / preRadius);
			
			//loft surface
			if (i == 1) {
				MString preCircle = curGeometry;
				preCircle += "Circle";
				preCircle += (i - 1);
				loft(preCircle, curCircle, curGeometry);
			}
			else {
				loft(curGeometry, curCircle);
			}
		}
		}
		return MStatus::kSuccess;
	}

	//get points in this curve
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

	/*if (manager->number_of_strokes % 3 == 0) manager->end();
	manager->number_of_strokes = nCurves;*/

	//end the last solution.
	if (mode == 1)
		manager->end();
	
	//initialize manager with current image dege information;
	if (manager->curt_solution == nullptr) {
		std::string s = manager->path;
		std::string delimiter = ".";
		std::string pathWitoutExtension = s.substr(0, s.find(delimiter));

		info = pathWitoutExtension.c_str();
		info += ".txt";
		MGlobal::displayInfo("Edge Path: " + info);
		vec3 camera = vec3(0.0, 0.0, -1.0);
		manager->init(camera, pathWitoutExtension + ".txt");
	}

	MString test = "TEST: ";
	test += (int)(manager->curt_solution->contours.size());
	MGlobal::displayInfo(test);

	for (int i = 0; i < numPoints; i++) {
		manager->update(points[i], true);
	}

	if (manager->curt_solution->compute()) {
		// todo
	}
	int shape = manager->curt_solution->shape;
	MString sp = "Shape: ";
	sp += shape;
	MGlobal::displayInfo(sp);

	Geometry* plane = (Geometry*)(manager->curt_solution->curt);
	
	if (!plane) {
		MGlobal::displayInfo("Geometry is not computed");
		return MStatus::kSuccess;
	}

	if (mode==2) {
		if (shape == Solution::Shape::CIRCLE) {
			MGlobal::displayInfo("Circle is computed");
			Circle* circle_plane = (Circle*)plane;
			vec3 origin = circle_plane->getOrigin();
			float radius = circle_plane->getRadius();
			vec3 normal = circle_plane->getNormal();
	
			MString circleName = curGeometry;
			circleName += "Circle0";
			drawCircle(origin, normal, radius, circleName);
 
		}
		else if (shape == Solution::Shape::SQUARE) {
			MGlobal::displayInfo("Square is not computed");
		}
		else {
			info = "this shape is not supported!";
			MGlobal::displayInfo(info);
		}
	}else if (mode == 3) {
		MGlobal::displayInfo("Extruding");

		//int index = nCurves/ 3; //circle index
		//MString thirdStroke = "curve";
		//thirdStroke += nCurves;//the third curve name

		//build temp plane
		Geometry* pre_plane = (manager->curt_solution->history[0]);
		Geometry* curt_plane = pre_plane;

		//build the first plane for cube: or cube
		if (shape == Solution::Shape::SQUARE) {

			Square* pre_square_plane = (Square*) pre_plane;
			
			MString squareName = curGeometry;
			squareName += "Square0";
			float length = pre_square_plane->getLength();
			float width = pre_square_plane->getWidth();
			vec3 origin = pre_square_plane->getOrigin();
			vec3 normal = pre_square_plane->getNormal();
			std::vector<vec3> vertices = pre_square_plane->getVertices();
			drawSquare(vertices, curGeometry);
		}

		for (int i = 1; i < manager->curt_solution->history.size(); i++) {
			// Done:: change to list of circle
			curt_plane = (manager->curt_solution->history[i]);

			vec3 preOrigin = pre_plane->getOrigin();
			vec3 curOrigin = curt_plane->getOrigin();
			vec3 preNormal = pre_plane->getNormal();
			vec3 curNormal = curt_plane->getNormal();

			float scaleRatio = 0;
			if (shape == Solution::Shape::CIRCLE) {
				float preRadius = pre_plane->getRadius();
				float curRadius = curt_plane->getRadius();
				scaleRatio = (curRadius / preRadius) * (curRadius / preRadius);
				// TEST START
				MString curCircle = curGeometry;
				curCircle += "Circle";
				curCircle += i;

				//draw current circle;
				drawCircle(curOrigin, curNormal, curRadius, curCircle);

				//loft surface
				if (i == 1) {
					MString preCircle = curGeometry;
					preCircle += "Circle";
					preCircle += (i-1);
					loft(preCircle, curCircle, curGeometry);
				}
				else {
					loft(curGeometry, curCircle);
				}
			}
			else if (shape == Solution::Shape::SQUARE) {

				Square* curt_square_plane = (Square*)curt_plane;
				float lastLength = pre_plane->getLength();
				float curLength = curt_square_plane->getLength();
				scaleRatio = (curLength / lastLength) * (curLength / lastLength);
				// update pre_circle
				pre_plane = curt_plane;
				int startIdx = -1;
				int endIdx = -1;

				vec3 translateW = curOrigin - preOrigin;//world 
				//float angleZ = glm::degrees(glm::acos(glm::dot(glm::normalize(vec3(preNormal.x, preNormal.y, preNormal.z)),glm::normalize(vec3(curNormal.x, curNormal.y, preNormal.z)))));
				//float angleY = glm::degrees(glm::acos(glm::dot(glm::normalize(vec3(preNormal.x, 0, preNormal.z)), glm::normalize(vec3(curNormal.x, 0, curNormal.z)))));
				//float angleX = glm::degrees(glm::acos(glm::dot(glm::normalize(vec3(0, preNormal.y, preNormal.z)), glm::normalize(vec3(0, curNormal.y, curNormal.z)))));
				vec3 rotationL = vec3(0, 0, 0);
				vec3 scaleL = vec3(scaleRatio, scaleRatio, scaleRatio);
				extrude(curGeometry, startIdx, endIdx, translateW, rotationL, scaleL);

				//Square* curt_plane = (Square*)curt_plane;
				//MString curSquare = curGeometry;
				//curSquare += "Square";
				//curSquare += i;
				//float curLength = curt_plane->getLength();
				//float curWidth = curt_plane->getWidth();
				////draw current square;
				//drawSquare(curOrigin, curNormal, curLength, curWidth, curSquare);

				////loft surface
				//if (i == 1) {
				//	MString preCircle = curGeometry;
				//	preCircle += "Circle";
				//	preCircle += (i - 1);
				//	loft(preCircle, curSquare, curGeometry);
				//}
				//else {
				//	loft(curGeometry, curSquare);
				//}
			}
		
		}

		//rename the curve
		MString cmd = "rename curve1 ";
		cmd += curGeometry + "curve1;";
		cmd += "rename curve2 ";
		cmd += curGeometry + "curve2;";
		cmd += "rename curve3 ";
		cmd += curGeometry + "curve3;";
		MGlobal::executeCommand(cmd, true);

		
	}

	return MStatus::kSuccess;
}

vec3 ThreeSweepCmd::convertCoordinates(vec3 point, bool reverse) {
	if (!reverse) return vec3(point.z, point.x, point.y);
	return vec3(point.y, point.z, point.x);
}

void ThreeSweepCmd::drawInitialCylinder(float radius, vec3 origin, vec3 ax, int sx, MString name) {

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
	cmd += ax.z;
	cmd += " -n ";
	cmd += name;
	cmd += "; move -r ";
	cmd += origin.x;
	cmd += " ";
	cmd += origin.y;
	cmd += " ";
	cmd += origin.z;

	MGlobal::displayInfo(cmd);
	MGlobal::executeCommand(cmd, true);
}

void ThreeSweepCmd::drawInitialCube(float length, float width, vec3 origin, vec3 ax, vec3 rotation, MString name) {
	MString cmd = "";
	cmd = "polyCube -w ";
	cmd += length;
	cmd += " -d ";
	cmd += width;
	cmd += " -h 0.0001 -ax ";
	cmd += ax.x;
	cmd += " ";
	cmd += -ax.y;
	cmd += " ";
	cmd += ax.z;
	cmd += " -n ";
	cmd += name;
	cmd += "; move -r ";
	cmd += origin.x;
	cmd += " ";
	cmd += origin.y;
	cmd += " ";
	cmd += origin.z;
	cmd += "; rotate -r -eu -fo -ws ";
	cmd += rotation.x;
	cmd += " ";
	cmd += rotation.y;
	cmd += " ";
	cmd += rotation.z;

	MGlobal::displayInfo(cmd);
	MGlobal::executeCommand(cmd, true);
}

void ThreeSweepCmd::extrude(MString objName, int startIdx, int endIdx, vec3 translate, vec3 rotation, vec3 scale) {
	
	MString cmd = "PolyExtrude; polyExtrudeFacet -constructionHistory 1 -keepFacesTogether 1 -pvx ";
	cmd += "0 -pvy 1 -pvz 0 -divisions 1 -twist 0 -taper 1 -off 0 -thickness 0 -smoothingAngle 0";
	cmd +=" -t ";
	cmd += translate.x;
	cmd += " ";
	cmd += translate.y;
	cmd += " ";
	cmd += translate.z; 
	cmd += " -lr ";
	cmd += rotation.x;
	cmd += " ";
	cmd += rotation.y;
	cmd += " ";
	cmd += rotation.z;
	cmd += " -ls ";
	cmd += scale.x;
	cmd += " ";
	cmd += scale.y;
	cmd += " ";
	cmd += scale.z;
	
	if (startIdx >= 0 && endIdx >= 0) {
		cmd += " ";
		cmd += objName;
		cmd += ".f[";
		cmd += startIdx;
		cmd += ":";
		cmd += endIdx;
		cmd += "]";
	}

	MGlobal::displayInfo(cmd);
	MGlobal::executeCommand(cmd, true);
}

void ThreeSweepCmd::extrudeAlongCurve(MString objName, int startIdx, int endIdx, MString curve, vec3 scale) {
	MString cmd = "PolyExtrude; polyExtrudeFacet -constructionHistory 1 -keepFacesTogether 1 -pvx ";
	cmd += "0 -pvy 1 -pvz 0 -divisions 1 -twist 0 -taper 1 -off 0 -thickness 0 -smoothingAngle 0";
	cmd += " -ls ";
	cmd += scale.x;
	cmd += " ";
	cmd += scale.y;
	cmd += " ";
	cmd += scale.z;
	cmd += " -inputCurve ";
	cmd += curve;
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

int ThreeSweepCmd::preProcess(MString path) {

        std::string s = path.asChar();
        std::string delimiter = "/";
        std::string pathPre = s.substr(0, s.find_last_of(delimiter));
        std::string cmd = pathPre.append("/EdgeDetectionInteractive.exe ");
        cmd.append(path.asChar());

        MString info = cmd.c_str();
        MGlobal::displayInfo(info);

        int retCode = system(cmd.c_str());
        return retCode;
}

glm::vec3 aimY(glm::vec3 vec);
void ThreeSweepCmd::drawCircle(vec3 origin, vec3 normal, float radius, MString name) {
	MString cmd = "circle -r ";
	cmd += radius;
	cmd += " -nr ";
	cmd += normal.x;
	cmd += " ";
	cmd += normal.y;
	cmd += " ";
	cmd += normal.z;
	cmd += " -c ";
	cmd += origin.x;
	cmd += " ";
	cmd += origin.y;
	cmd += " ";
	cmd += origin.z;
	cmd += " -n ";
	cmd += name;
	MGlobal::displayInfo(cmd);
	MGlobal::executeCommand(cmd, true);

	glm::vec3 rotateAngle = aimY(normal);
	cmd = "xform -cp;";
	cmd += "manipPivot -o ";
	cmd += rotateAngle.x;
	cmd += " ";
	cmd += rotateAngle.y;
	cmd += " ";
	cmd += rotateAngle.z;
	cmd += " -pin true;";
	MGlobal::displayInfo(cmd);
	MGlobal::executeCommand(cmd, true);
}

void ThreeSweepCmd::drawSquare(vec3 origin, vec3 normal, float length, float width, MString name) {
	MString cmd = "nurbsSquare -sl1 ";
	cmd += length;
	cmd += " -sl2 ";
	cmd += width;
	cmd += " -nr ";
	cmd += normal.x;
	cmd += " ";
	cmd += normal.y;
	cmd += " ";
	cmd += normal.z;
	cmd += " -c ";
	cmd += origin.x;
	cmd += " ";
	cmd += origin.y;
	cmd += " ";
	cmd += origin.z;
	cmd += " -n ";
	cmd += name;
	MGlobal::displayInfo(cmd);
	MGlobal::executeCommand(cmd, true);
}

void ThreeSweepCmd::drawSquare(std::vector<vec3> points, MString name){
	MString cmd = "curve -d 1 -p ";
	cmd += points[0].x;
	cmd += " ";
	cmd += points[0].y;
	cmd += " ";
	cmd += points[0].z;
	cmd += " -p ";
	cmd += points[1].x;
	cmd += " ";
	cmd += points[1].y;
	cmd += " ";
	cmd += points[1].z;
	cmd += " -n side;";
	MGlobal::executeCommand(cmd, true);//side1
	MGlobal::executeCommand(cmd, true);//side2

	vec3 moveVec = points[2] - points[1];
	cmd = "move -r ";
	cmd += moveVec.x;
	cmd += " ";
	cmd += moveVec.y;
	cmd += " ";
	cmd += moveVec.z;
	cmd += " side2;";
	cmd += "loft -ch 1 -u 1 -c 0 -ar 1 -d 3 -ss 1 -rn 0 -po 0 -rsn false -n nurbsSquare1 ";
	cmd += "side1 side2;";
	cmd += "delete side1 side2;";
	MGlobal::executeCommand(cmd, true); //move and loft

	cmd = "nurbsToPoly -pt 1 -n ";
	cmd += name;
	cmd += " nurbsSquare1;";
	
	MString reduceCmd = "ReducePolygon; setAttr \"polyReduce1.termination\" 1; setAttr \"polyReduce1.vertexCount\" 4;";//set reduction method to vertex
	cmd += reduceCmd;
	//cmd += "delete nurbsSquare1;";
	MGlobal::executeCommand(cmd, true); //toPolygon and reduce
}

void ThreeSweepCmd::loft(MString curve1, MString curve2, MString objName) {
	MString cmd = "loft -ch 1 -u 1 -c 0 -ar 1 -d 3 -ss 1 -rn 0 -po 0 -rsn false -n ";
	cmd += objName;
	cmd += " ";
	cmd += curve1;
	cmd += " "; 
	cmd += curve2;
	
	MGlobal::displayInfo(cmd);
	MGlobal::executeCommand(cmd, true);
}

void ThreeSweepCmd::loft(MString objName, MString curve1) {
	MString cmd = "select -r ";
	cmd += objName;
	cmd += "; select -tgl ";
	cmd += curve1;
	cmd += "; Loft;";
	MGlobal::displayInfo(cmd);
	MGlobal::executeCommand(cmd, true);
}

glm::vec3 aimY(glm::vec3 vec) {
	glm::vec3 out;
	float xAngle, zAngle, xyLength, vecLength;
	xyLength = sqrt((vec.x) * (vec.x) +
		(vec.y) * (vec.y));
	vecLength = sqrt((vec.x) * (vec.x) +
		(vec.y) * (vec.y) +
		(vec.z) * (vec.z));

	// xyLength will be zero when vec is pointing
	// along the +z or -z axis
	if (xyLength == 0)
		zAngle = (vec.x) > 0 ? glm::radians(90.0) : glm::radians(-90.0);
	else
		zAngle = acos((vec.y) / xyLength);

	xAngle = -acos(xyLength / vecLength);

	xAngle = (vec.z) > 0 ? xAngle : -xAngle;
	out[0] = glm::degrees(xAngle);

	zAngle = (vec.x) > 0 ? -zAngle : zAngle;
	out[2] = glm::degrees(zAngle);
	return out;
}

//void ThreeSweepCmd::createGeometry() {

//}