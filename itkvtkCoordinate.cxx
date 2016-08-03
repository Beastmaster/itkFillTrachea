/*
Author: QIN Shuo
Date: 2016/7/14
Description:
	Transform itk coordinate to vtk coordinate
Solution:
	fail to apply right transform
*/

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImage.h"
#include "itkPointSet.h"
#include "itkOrientImageFilter.h"

#include "vtkSmartPointer.h"
#include <vtkImageData.h>
#include "vtkPointData.h"
#include "vtkDataArray.h"
#include "vtkPointSet.h"
#include "vtkNIFTIImageWriter.h"


/*
Input:
	input_img: An ITK image pointer
Ouput:
	An VTK vtkImageData
*/
vtkSmartPointer<vtkImageData> itk2vtkCoordinate(itk::Image< int, 3 >::Pointer input_img)
{
	typedef itk::Image< int, 3 > ImageType;

	 //Re-orient image
	typedef itk::OrientImageFilter<ImageType, ImageType> ReOrientorType;
	auto reOrientor = ReOrientorType::New();
	reOrientor->UseImageDirectionOn();
	reOrientor->SetDesiredCoordinateOrientation(itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RAI);
	reOrientor->SetInput(input_img);
	reOrientor->Update();
	

	// create image
	auto itkImg = reOrientor->GetOutput();
	unsigned int dimensions = itkImg->GetImageDimension();
	auto region = itkImg->GetLargestPossibleRegion();
	auto size = region.GetSize();
	auto spacing = itkImg->GetSpacing();
	auto origin = itkImg->GetOrigin();
	auto direction = itkImg->GetDirection();

	// create vtkimagedata
	auto vtkImg = vtkSmartPointer<vtkImageData>::New();
	vtkImg->SetDimensions(size[0], size[1], size[2]);
	vtkImg->SetSpacing(spacing[0], spacing[1], spacing[2]);
	vtkImg->SetOrigin(origin[0], origin[1], origin[2]);
	vtkImg->AllocateScalars(VTK_INT, 1);
	//vtkImg->AllocateScalars(VTK_UNSIGNED_CHAR, 1);

	auto scalar = vtkImg->GetScalarPointer();
	long sz = sizeof( int)*size[0] * size[1] * size[2];
	std::memcpy(scalar, itkImg->GetBufferPointer(), sz);

	//auto writer = vtkSmartPointer<vtkNIFTIImageWriter>::New();
	//writer->SetFileName("E:/test/vtktt.nii");
	//writer->SetInputData(vtkImg);
	//writer->Write();

	return vtkImg;
}



