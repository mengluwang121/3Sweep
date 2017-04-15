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
	void drawCircle(Circle* result, int numStrokes);
	void ThreeSweepCmd::drawInitialCylinder(float radius, vec3 origin, vec3 ax, int sx, MString name);
	void ThreeSweepCmd::drawInitialCube(float l, vec3 origin, vec3 ax, vec3 rotation, MString name);
	void extrude(MString objName, int startIdx, int endIdx, vec3 translate, vec3 rotation, vec3 scale);
	static Manager* manager;
};

#endif