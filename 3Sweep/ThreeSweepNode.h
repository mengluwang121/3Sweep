#pragma once
#include <maya/MFnMesh.h>
#include <maya/MPxNode.h>
#include <maya/MDataBlock.h>
#include "glm\glm.hpp"
#include "glm\vector_relational.hpp"
#include "glm\vec3.hpp"
using namespace glm;


class ThreeSweepNode : public MPxNode
{
public:
	ThreeSweepNode() { initialize(); };
	virtual                 ~ThreeSweepNode() {};
	virtual MStatus compute(const MPlug& plug, MDataBlock& data);
	static  void*   creator();
	static  MStatus initialize();

	static MObject nSamplePoints;
	static MObject outputMesh;

	static MTypeId  id;

protected:
	MStatus createMesh(vec3 start, vec3 end, MPointArray& points, MIntArray& faceCounts, MIntArray& faceConnects);
};
