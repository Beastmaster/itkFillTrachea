/*
/*
Author: QIN Shuo
Date: 2016/7/25
Description:
	Workflow
*/


#include <string>
#include <stdio.h>
#include <vector>


#include "itkImage.h"
#include "vtkSmartPointer.h"
#include "vtkImageData.h"
#include "vtkNIFTIImageWriter.h"
#include "vtkNIFTIImageReader.h"
#include "vtkMarchingCubes.h"
#include "vtkPolyData.h"
// for visi
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>

itk::Image< int, 3 >::Pointer itk2vtkReadDicom(const char* dirname);
vtkSmartPointer<vtkImageData> itk2vtkCoordinate(itk::Image<  int, 3 >::Pointer input_img);
int itkRegistrationTest(std::string moving, std::string fix, std::string warp, std::string preffix, std::string brain, std::string warp_brain);
int itkTransform(std::vector<double*>& list, std::string filename);
int fillHoleFilter(std::string ori_name, std::string brain, std::string surface,int threshold);
vtkSmartPointer<vtkPolyData> Marching_Connected(vtkImageData* input, double strip_value);



int main(int argc, char** argv)
{

	std::string dicomdir = "E:/test/head-dicom";
	std::string niftiname = "E:/test/head.nii";

	// read dicom files
	auto itk_img1 = itk2vtkReadDicom(dicomdir.c_str());
	// convert to vtkimagedata
	auto vtk_img1 = itk2vtkCoordinate(itk_img1);
	// write to nifti file
	auto writer = vtkSmartPointer<vtkNIFTIImageWriter>::New();
	writer->SetFileName(niftiname.c_str());
	writer->SetInputData(vtk_img1);
	writer->Update();

	// registration
	std::string moving = "E:/test/reference_brain_res.nii";
	std::string fix = "E:/test/head.nii";
	std::string warp = "E:/test/moved_reference.nii";
	std::string moving_brain = "E:/test/reference_brain_aal.nii";
	std::string warp_brain = "E:/test/moved_reference_brain.nii";
	std::string preffix = "E:/test/moved_transform";
	//itkRegistrationTest( moving, fix, warp, preffix,moving_brain,warp_brain);

	//transform points
	std::vector<double*> list;
	double p1[] = { 0.1, 0.2, 0.3 };
	double p2[] = { 0.1, 0.2, 0.3 };
	double p3[] = { 0.1, 0.2, 0.3 };
	list.push_back(p1); list.push_back(p2); list.push_back(p3);
//	itkTransform(list, preffix+"Affine.txt");

	// fill hole
	std::string filled_name = "E:/test/filled_skin.nii";  // use this file to extract skin surface
//	fillHoleFilter(fix, warp_brain, filled_name,50);

	//run marchingcubes
	auto reader = vtkSmartPointer<vtkNIFTIImageReader>::New();
	reader->SetFileName(filled_name.c_str());
	reader->Update();

	auto suf = Marching_Connected(reader->GetOutput(), 10);



	//skin actor
	auto skin_mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	skin_mapper->SetInputData(suf);
	auto skin_actor = vtkSmartPointer<vtkActor>::New();
	skin_actor->SetMapper(skin_mapper);
	vtkSmartPointer<vtkRenderer> renderer =
		vtkSmartPointer<vtkRenderer>::New();
	renderer->AddActor(skin_actor);

	vtkSmartPointer<vtkRenderWindow> renwin =
		vtkSmartPointer<vtkRenderWindow>::New();
	renwin->AddRenderer(renderer);

	vtkSmartPointer<vtkRenderWindowInteractor> iren =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();
	iren->SetRenderWindow(renwin);
	iren->Initialize();
	iren->Start();


	return 0;
}












