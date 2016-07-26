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

*/


#include <string>

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkPoint.h"

#include "vtkImageData.h"
#include "vtkSmartPointer.h"
#include "vtkNIFTIImageReader.h"
#include "itkIndex.h"


int FindVTKCoordinateInITK(std::string filename, double* in, double* out)
{
	auto vtkreader = vtkSmartPointer<vtkNIFTIImageReader>::New();
	vtkreader->SetFileName(filename.c_str());
	vtkreader->Update();

	auto vtk_buf = vtkreader->GetOutput();

	unsigned long id_index = vtk_buf->FindPoint(in);

	auto itkreader = itk::ImageFileReader< itk::Image<int, 3> >::New();
	itkreader->SetFileName(filename);
	itkreader->Update();

	auto Image = itkreader->GetOutput();

	typedef double PixelType;
	const unsigned int Dimension = 3;
	typedef itk::Point<PixelType, 3> PointType;

	PointType pp;

	itk::Index<1> index;
	index.Fill(id_index);

	

}



int FindITKCoordinateInVTK(std::string filename, double* in, double* out)
{





}

















