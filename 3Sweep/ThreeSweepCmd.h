#ifndef Create3SweepCmd_H_
#define Create3SweepCmd_H_

#include <maya/MPxCommand.h>
#include <maya/MSyntax.h>
#include <maya/MDistance.h>
#include <maya/MVector.h>
#include <maya/MArgList.h>
#include <maya/MArgDatabase.h>
#include <maya/MGlobal.h>
#include <vector>
#include <string>
#include <cmath>
#include "backend\Manager.h"
#include "backend\Test.h"
class ThreeSweepCmd : public MPxCommand
{
public: 
	ThreeSweepCmd();
	~ThreeSweepCmd();
	static MSyntax newSyntax();
	static void* creator() { return new ThreeSweepCmd(); }
	MStatus doIt(const MArgList& args);
	int preProcess(MString path);
	
	void drawInitialCylinder(float radius, vec3 origin, vec3 ax, int sx, MString name);
	void drawInitialCube(float l, vec3 origin, vec3 ax, vec3 rotation, MString name);
	void extrude(MString objName, int startIdx, int endIdx, vec3 translate, vec3 rotation, vec3 scale);
	void extrudeAlongCurve(MString objName, int startIdx, int endIdx, MString curve, vec3 scale);
	void ThreeSweepCmd::drawCircle(vec3 origin, vec3 normal, float radius, MString name);
	void ThreeSweepCmd::loft(MString curve1, MString curve2, MString objName);
	void ThreeSweepCmd::loft(MString objName, MString curve1);
	vec3 convertCoordinates(vec3 point, bool reverse);
	static Manager* manager;
};

#endif