/*
Author: QIN Shuo
Date: 2016/7/26
Description:
	Search the corresponding coordinate in vtk and itk system

WorkFlow:
A. Given vtk coordinate to find ITK coordinate
	1. Read VTK and ITK image
	2. Find the point index in vtkImageData
	3. Use the index to get the coordinate in ITK image

B. Given itk coordinate to find VTK coordinate
	same as A.

Must specify filename


Attention:
	This method havn't tested
	
*/


#include <string>

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkPoint.h"

#include "vtkImageData.h"
#include "vtkSmartPointer.h"
#include "vtkNIFTIImageReader.h"
#include "itkIndex.h"


/*
 Got VTK coordinate, transform to ITK coordiante
*/
int FindVTKCoordinateInITK(std::string filename, double* in, double* out)
{
	auto vtkreader = vtkSmartPointer<vtkNIFTIImageReader>::New();
	vtkreader->SetFileName(filename.c_str());
	vtkreader->Update();

	auto vtk_buf = vtkreader->GetOutput();

	long long id_index = vtk_buf->FindPoint(in);

	auto itkreader = itk::ImageFileReader< itk::Image<int, 3> >::New();
	itkreader->SetFileName(filename);
	itkreader->Update();

	auto itkImage = itkreader->GetOutput();

	typedef double PixelType;
	const unsigned int Dimension = 3;
	typedef itk::Point<PixelType, 3> PointType;
	typedef itk::Image<PixelType, 3> ImageType;

	PointType pp;

	ImageType::IndexType index;
	ImageType::OffsetValueType offset;
	offset = id_index;

	index = itkImage->ComputeIndex(offset);
	itkImage->TransformIndexToPhysicalPoint(index,pp);


	for ( unsigned char i = 0; i < 3; i++)
		out[i] = pp[i];

	return 0;
}


/*
Got ITK coordinate, transform to VTK coordiante
*/
int FindITKCoordinateInVTK(std::string filename, double* in, double* out)
{
	typedef double PixelType;
	const unsigned int Dimension = 3;
	typedef itk::Point<PixelType, 3> PointType;
	typedef itk::Image<PixelType, 3> ImageType;

	auto itkreader = itk::ImageFileReader<ImageType>::New();
	itkreader->SetFileName(filename.c_str());
	itkreader->Update();

	auto itk_buf = itkreader->GetOutput();

	ImageType::IndexType index;
	PointType pp;
	for (unsigned char i = 0; i < 3; i++)
		pp[i] = in[i];
	itk_buf->TransformPhysicalPointToIndex(pp,index);
	ImageType::OffsetValueType offset;
	offset = itk_buf->ComputeOffset(index);
	
	long long vtk_id = offset;

	auto vtkreader = vtkSmartPointer<vtkNIFTIImageReader>::New();
	vtkreader->SetFileName(filename.c_str());
	vtkreader->Update();

	auto vtk_buf = vtkreader->GetOutput();

	vtk_buf->GetPoint(vtk_id,out);
	return 0;
}

















