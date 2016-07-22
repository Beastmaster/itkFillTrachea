/*
Author: QIN Shuo
Date: 2016/7/14
Description:
	Transform itk coordinate to vtk coordinate
Solution:
	
*/

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImage.h"
#include "itkPointSet.h"


#include "vtkSmartPointer.h"
#include <vtkImageData.h>
#include "vtkPointData.h"
#include "vtkDataArray.h"
#include "vtkPointSet.h"
#include "vtkNIFTIImageWriter.h"


/*


*/
vtkSmartPointer<vtkImageData> itk2vtkCoordinate(itk::Image< unsigned, 3 >::Pointer input_img)
{
	typedef itk::Image< unsigned, 3 > ImageType;

	// create image
	auto itkImg = input_img;
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
	vtkImg->AllocateScalars(VTK_UNSIGNED_INT, 1);
	
	auto scalar = vtkImg->GetScalarPointer();
	std::memcpy(scalar, itkImg->GetBufferPointer(), sizeof(unsigned)*size[0] * size[1] * size[2]);

	return vtkImg;
}



