/*
Author: QIN Shuo
Date: 2016/7/25
Description:
	marching cubes and extract largest connected surface

*/


#include "vtkSmartPointer.h"
#include "vtkMarchingCubes.h"
#include "vtkPolyDataConnectivityFilter.h"
#include "vtkImageData.h"
#include "vtkGeometryFilter.h"
#include "vtkSmoothPolyDataFilter.h"


/*
Input:
	input: input vtkimagedata
	strip_value: strip value
Output:
	striped polydata, a surface
*/
vtkSmartPointer<vtkPolyData> Marching_Connected(vtkImageData* input , double strip_value)
{

	auto marchingCubes = vtkSmartPointer<vtkMarchingCubes>::New();
#if VTK_MAJOR_VERSION <= 5
	marchingCubes->SetInput(input);
#else
	marchingCubes->SetInputData(input);
#endif
	//marchingCubes->SetNumberOfContours(2);
	marchingCubes->SetValue(0, strip_value);
	//marchingCubes update
	marchingCubes->Update();
	auto buff = vtkSmartPointer<vtkPolyData>::New();
	buff = marchingCubes->GetOutput();


	vtkSmartPointer<vtkPolyDataConnectivityFilter> connectivityFilter =
		vtkSmartPointer<vtkPolyDataConnectivityFilter>::New();
#if VTK_MAJOR_VERSION <= 5
	connectivityFilter->SetInput(m_ImageSkin);
#else
	connectivityFilter->SetInputData(buff);
#endif
	connectivityFilter->SetExtractionModeToLargestRegion();
	connectivityFilter->Update();

	vtkSmartPointer<vtkGeometryFilter> geometryFilter =
		vtkSmartPointer<vtkGeometryFilter>::New();
	geometryFilter->SetInputData(connectivityFilter->GetOutput());
	geometryFilter->Update();
	buff = geometryFilter->GetOutput();
	
	// smooth 
	auto smoothfilter = vtkSmartPointer<vtkSmoothPolyDataFilter>::New();
	smoothfilter->SetInputData(buff);
	smoothfilter->SetConvergence(10);
	smoothfilter->BoundarySmoothingOn();
	smoothfilter->FeatureEdgeSmoothingOn();
	smoothfilter->Update();

	
	buff = smoothfilter->GetOutput();

	return buff;
}


