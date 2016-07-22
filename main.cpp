
#include <string>
#include <stdio.h>



#include "itkImage.h"
#include "vtkSmartPointer.h"
#include "vtkImageData.h"


itk::Image< unsigned, 3 >::Pointer itk2vtkReadDicom(const char* dirname);
vtkSmartPointer<vtkImageData> itk2vtkCoordinate(itk::Image< unsigned, 3 >::Pointer input_img);
int itkRegistrationTest();

int main(int argc, char** argv)
{
	std::string dicomdir = "E:/test/head-dicom";

	// read dicom files
	auto itk_img1 = itk2vtkReadDicom(dicomdir.c_str());

	auto vtk_img1 = itk2vtkCoordinate(itk_img1);

	

	return 0;
}












