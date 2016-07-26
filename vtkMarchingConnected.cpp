/*
Author: QIN Shuo
Date: 2016/7/25
Description:
	marching cubes and extract largest connected surface

*/

#include <functional>

#include "vtkSmartPointer.h"
#include "vtkMarchingCubes.h"
#include "vtkPolyDataConnectivityFilter.h"
#include "vtkImageData.h"
#include "vtkGeometryFilter.h"
#include "vtkSmoothPolyDataFilter.h"
#include "vtkImageCast.h"
#include "vtkImageGaussianSmooth.h"

/*
Input:
	input: input vtkimagedata, typically a thresholded image (fillHole)
	strip_value: strip value
Output:
	striped polydata, a surface
*/
vtkSmartPointer<vtkPolyData> Marching_Connected(vtkImageData* input , double strip_value)
{

	auto pre_process = [input]()
	{
		auto caster = vtkSmartPointer<vtkImageCast>::New();
		caster->SetInputData(input);
		caster->SetOutputScalarTypeToFloat();
		caster->Update();

		//====  smooth   ====//
		vtkSmartPointer<vtkImageGaussianSmooth> gaussianSmoothFilter =
			vtkSmartPointer<vtkImageGaussianSmooth>::New();
		gaussianSmoothFilter->SetInputData(caster->GetOutput());
		gaussianSmoothFilter->Update();

		auto new_img = vtkSmartPointer<vtkImageData>::New();
		new_img = gaussianSmoothFilter->GetOutput();

		return new_img;
	};
	auto temp = pre_process();

	auto marchingCubes = vtkSmartPointer<vtkMarchingCubes>::New();
#if VTK_MAJOR_VERSION <= 5
	marchingCubes->SetInput(temp);
#else
	marchingCubes->SetInputData(temp);
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
	//smoothfilter->Update();

	
	//buff = smoothfilter->GetOutput();

	return buff;
}


