
#include <maya/MFnMesh.h>

#include <maya/MFloatPointArray.h>
#include <maya/MIntArray.h>
#include <maya/MDoubleArray.h>
#include <maya/MPointArray.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnNumericAttribute.h>

#include <maya/MPxNode.h>
#include <maya/MObject.h>
#include <maya/MDataBlock.h>
#include <maya/MFnMeshData.h>

#include <maya/MGlobal.h>
#include "ThreeSweepNode.h"

MStatus returnStatus;

#define McheckErr(stat,msg)                     \
        if ( MS::kSuccess != stat ) {   \
                cerr << msg;                            \
                return MS::kFailure;            \
        }

MObject ThreeSweepNode::nSamplePoints;
MObject ThreeSweepNode::outputMesh;

MTypeId ThreeSweepNode::id(0x80000);

void* ThreeSweepNode::creator()
{
	return new ThreeSweepNode;
}

MStatus ThreeSweepNode::initialize()
{

	MGlobal::displayInfo("initialize");

	MFnNumericAttribute numAttr;
	MFnUnitAttribute unitAttr;
	MFnTypedAttribute typedAttr;

	MStatus returnStatus;

	//create attributes
	ThreeSweepNode::nSamplePoints = numAttr.create("nSamplePoints", "sp",
		MFnNumericData::kDouble,
		20.0, &returnStatus);
	McheckErr(returnStatus, "ERROR creating nSamplePoints attribute\n");
	unitAttr.setStorable(true);

	ThreeSweepNode::outputMesh = typedAttr.create("outputMesh", "out",
		MFnData::kMesh,
		&returnStatus);
	McheckErr(returnStatus, "ERROR creating output mesh attribute\n");
	typedAttr.setStorable(false);


	//add attributes
	MGlobal::displayInfo("add attributes");
	returnStatus = addAttribute(ThreeSweepNode::nSamplePoints);
	McheckErr(returnStatus, "ERROR adding resolution attribute\n");
	
	returnStatus = addAttribute(ThreeSweepNode::outputMesh);
	McheckErr(returnStatus, "ERROR adding outputMesh attribute\n");


	//add affects
	returnStatus = attributeAffects(ThreeSweepNode::nSamplePoints,
		ThreeSweepNode::outputMesh);
	McheckErr(returnStatus, "ERROR in attributeAffects\n");

	return MS::kSuccess;
}

MStatus ThreeSweepNode::compute(const MPlug& plug, MDataBlock& data)
{
	MStatus returnStatus;
	MGlobal::displayInfo("compute");
	if (plug == outputMesh) {

		//LSystem system;
		double nSamplePoints_d = 20;
		//get data
		MDataHandle nSamplePointsData = data.inputValue(nSamplePoints, &returnStatus);
		McheckErr(returnStatus, "Error getting nSamplePoints data handle\n");
		nSamplePoints_d = nSamplePointsData.asDouble();

		//get output
		MDataHandle outputHandle = data.outputValue(outputMesh, &returnStatus);
		McheckErr(returnStatus, "Error getting output mesh handle\n");

		MFnMeshData dataCreator;
		MObject newOutputData = dataCreator.create(&returnStatus);
		McheckErr(returnStatus, "ERROR creating outputData");

		//create mesh for the node
		MPointArray points;
		MIntArray faceCounts;
		MIntArray faceConnects;
		for (int j = 0; j < 100000; j++) {
			vec3 start = vec3(0, 0, 0);
			vec3 end = vec3(1, 1, 1);
			createMesh(start, end, points, faceCounts, faceConnects);
		}

		MFnMesh meshFS;
		MObject newMesh = meshFS.create(points.length(), faceCounts.length(), points, faceCounts, faceConnects, newOutputData, &returnStatus);
		McheckErr(returnStatus, "ERROR creating outMesh");

		outputHandle.set(newOutputData);
		data.setClean(plug);
	}
	else
		return MS::kUnknownParameter;

	return MS::kSuccess;
}

MStatus ThreeSweepNode::createMesh(vec3 start, vec3 end, MPointArray& points, MIntArray& faceCounts, MIntArray& faceConnects)
{
	MStatus returnStatus;

	double r = 0.2;
	MPoint start_mp = MPoint(MVector(start[0], start[1], start[2]));
	MPoint end_mp = MPoint(MVector(end[0], end[1], end[2]));

	//CylinderMesh newCylinderMesh = CylinderMesh(start_mp, end_mp, r);
	//newCylinderMesh.appendToMesh(points, faceCounts, faceConnects);

	return MS::kSuccess;
}

MStatus MPxNode::dependsOn(const MPlug &, const MPlug &, bool &) const
{
	return MS::kUnknownParameter;
}