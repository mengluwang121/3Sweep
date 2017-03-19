//
// Copyright (C) 
// 
// File: pluginMain.cpp
//
// Author: Maya Plug-in Wizard 2.0
//
#include <maya/MGlobal.h>
#include <maya/MFnPlugin.h>
#include "ThreeSweepCmd.h"
#include <iostream>

MStatus initializePlugin( MObject obj )
//
//	Description:
//		this method is called when the plug-in is loaded into Maya.  It 
//		registers all of the services that this plug-in provides with 
//		Maya.
//
//	Arguments:
//		obj - a handle to the plug-in object (use MFnPlugin to access it)
//
{ 
	MStatus   status;
	MFnPlugin plugin( obj, "3Sweep", "2015", "Any");

	MGlobal::executeCommand("source \"" + plugin.loadPath() + "/3SweepUI.mel\"", true);

	status = plugin.registerUI("initialize3SweepUI","delete3SweepUI");
	if (!status) {
		status.perror("registerUI");
		return status;
	}

	status = plugin.registerCommand("ThreeSweepCmd", ThreeSweepCmd::creator);
	if (!status) {
		status.perror("registerCommand");
		return status;
	}

	return status;
}

MStatus uninitializePlugin( MObject obj)
//
//	Description:
//		this method is called when the plug-in is unloaded from Maya. It 
//		deregisters all of the services that it was providing.
//
//	Arguments:
//		obj - a handle to the plug-in object (use MFnPlugin to access it)
//
{
	MStatus   status;
	MFnPlugin plugin( obj );

	status = plugin.deregisterCommand("ThreeSweepCmd");
	if (!status) {
		status.perror("deregisterCommand");
		return status;
	}

	return status;
}
