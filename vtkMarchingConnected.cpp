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
	// exclude boundary
	auto m_Image = input;

	auto exclude_boundary = [m_Image]()
	{
		int* dims = m_Image->GetDimensions();
		for (int z = 0; z < dims[2]; z++)
		{
			for (int y = 0; y < dims[1]; y++)
			{
				float* pixel = static_cast<float*>(m_Image->GetScalarPointer(0, y, z)); pixel[0] = 0.0;
				pixel = static_cast<float*>(m_Image->GetScalarPointer(dims[0] - 1, y, z)); pixel[0] = 0.0;
			}
		}
		for (int x = 0; x < dims[0]; x++)
		{
			for (int y = 0; y < dims[1]; y++)
			{
				float* pixel = static_cast<float*>(m_Image->GetScalarPointer(x, y, 0)); pixel[0] = 0.0;
				pixel = static_cast<float*>(m_Image->GetScalarPointer(x, y, dims[2] - 1)); pixel[0] = 0.0;
			}
		}
		for (int x = 0; x < dims[0]; x++)
		{
			for (int z = 0; z < dims[2]; z++)
			{
				float* pixel = static_cast<float*>(m_Image->GetScalarPointer(x, 0, z)); pixel[0] = 0.0;
				pixel = static_cast<float*>(m_Image->GetScalarPointer(x, dims[1] - 1, z)); pixel[0] = 0.0;
			}
		}
	};

	auto pre_process = [m_Image]()
	{
		auto caster = vtkSmartPointer<vtkImageCast>::New();
		caster->SetInputData(m_Image);
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


