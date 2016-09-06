/*
Author: QIN Shuo
Date: 2016/7/25
Description:
	Workflow
*/


#include <string>
#include <stdio.h>
#include <vector>
#include <functional>


#include "itkImage.h"
#include "itkImageFileWriter.h"
#include "itkImageFileReader.h"
#include "vtkSmartPointer.h"
#include "vtkImageData.h"
#include "vtkNIFTIImageWriter.h"
#include "vtkNIFTIImageReader.h"
#include "vtkMarchingCubes.h"
#include "vtkPolyData.h"
#include "vtkSphereSource.h"
#include "vtkProperty.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
// for visi
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>

// stl test
#include "vtkSTLWriter.h"

itk::Image< int, 3 >::Pointer itk2vtkReadDicom(const char* dirname);
vtkSmartPointer<vtkImageData> itk2vtkCoordinate(itk::Image<  int, 3 >::Pointer input_img);
int itkRegistrationTest(std::string moving, std::string fix, std::string warp, std::string preffix, std::string brain, std::string warp_brain);
int itkTransform(std::vector<double*>& list, std::string filename);
int fillHoleFilter(std::string ori_name, std::string brain, std::string surface,int threshold);
vtkSmartPointer<vtkPolyData> Marching_Connected(vtkImageData* input, double strip_value);
int FindVTKCoordinateInITK(std::string filename, double* in, double* out);
int FindITKCoordinateInVTK(std::string filename, double* in, double* out);
int itkTransformImage(std::string transform_file, std::string in_filename, std::string out_filename);
vtkSmartPointer<vtkMatrix4x4> readITKTransform(std::string filename);

int main(int argc, char** argv)
{

	// registration
	std::string moving = "E:/test/reference_brain_res.nii";
	std::string fix = "E:/test/user_temp.nii";
	std::string warp = "E:/test/moved_reference.nii";
	std::string moving_brain = "E:/test/reference_brain_aal.nii";
	std::string warp_brain = "E:/test/moved_reference_brain.nii";
	std::string preffix = "E:/test/moved_transform";
	//itkRegistrationTest( moving, fix, warp, preffix,moving_brain,warp_brain);


	double pp[5][3] = {
		{ -74.28785565024133, 1.852807051680778, -57.697169892541396 },
		{ -45.71499559323904, -79.42181459334893, -26.760723328753894 },
		{ 4.470394992086342, -111.07725729165594, -59.13879565169253 },
		{ 60.48465217481051, -78.3687979299389, -26.61622868170861 },
		{ 90.38648776152144, -1.4701530046301838, -56.11362240402192 }
	};
	std::vector<double* > points;
	for (size_t i = 0; i < 5; i++)
	{
		points.push_back(pp[i]);
	}

	itkTransform(points, preffix+"Affine.txt");



	auto vtkreader = itk::ImageFileReader<itk::Image<int, 3> >::New();
	vtkreader->SetFileName(fix.c_str());
	vtkreader->Update();
	auto fix_image = vtkSmartPointer<vtkImageData>::New();
	fix_image = itk2vtkCoordinate(vtkreader->GetOutput());
	auto fix_poly = Marching_Connected(fix_image, 10);
	std::cout << "Marching 1 done" << std::endl;

	/*	
	//itkTransformImage(preffix + "Affine.txt", moving, "E:/test/resample_reference.nii");
	auto vtkreader2 = itk::ImageFileReader<itk::Image<int, 3> >::New();
	vtkreader2->SetFileName(moving.c_str()); //("E:/test/resample_reference.nii");// 
	vtkreader2->Update();
	auto moving_image = vtkSmartPointer<vtkImageData>::New();
	moving_image = itk2vtkCoordinate(vtkreader2->GetOutput());
	auto moving_poly = Marching_Connected(moving_image, 10);
	std::cout << "Marching 2 done" << std::endl;

	auto vtkreader3 = itk::ImageFileReader<itk::Image<int, 3> >::New();
	vtkreader3->SetFileName(warp.c_str());
	vtkreader3->Update();
	auto warp_image = vtkSmartPointer<vtkImageData>::New();
	warp_image = itk2vtkCoordinate(vtkreader3->GetOutput());
	auto warp_poly = Marching_Connected(warp_image, 10);
	std::cout << "Marching 3 done" << std::endl;


	vtkSmartPointer<vtkMatrix4x4> mat = readITKTransform(preffix + "Affine.txt");
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			std::cout << mat->GetElement(i,j) << ",";
		}
		std::cout << std::endl;
	}
	// transform
	auto transform = vtkSmartPointer<vtkTransform>::New();
	transform->SetMatrix(mat);
	transform->Update();

	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter =
		vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	transformFilter->SetInputData(moving_poly);
	transformFilter->SetTransform(transform);
	transformFilter->Update();
	moving_poly = transformFilter->GetOutput();




	auto mapper2 = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper2->SetInputData(moving_poly);
	auto actor2 = vtkSmartPointer<vtkActor>::New();
	actor2->SetMapper(mapper2);
	
	auto mapper3 = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper3->SetInputData(warp_poly);
	auto actor3 = vtkSmartPointer<vtkActor>::New();
	actor3->SetMapper(mapper3);
	*/
	auto mapper1 = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper1->SetInputData(fix_poly);
	auto actor1 = vtkSmartPointer<vtkActor>::New();
	actor1->SetMapper(mapper1);

	auto renderer = vtkSmartPointer<vtkRenderer>::New();
	auto renderer2 = vtkSmartPointer<vtkRenderer>::New();
	auto renWin = vtkSmartPointer<vtkRenderWindow>::New();
	auto interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renWin->AddRenderer(renderer);
	renderer->SetViewport(0, 0, 0.5, 1);
	renWin->AddRenderer(renderer2);
	renderer2->SetViewport(0.5, 0, 1, 1);
	interactor->SetRenderWindow(renWin);
	
	renderer->AddActor(actor1);
	
	for (size_t i = 0; i < 5; i++)
	{
		auto sphereSrc = vtkSmartPointer<vtkSphereSource>::New();
		sphereSrc->SetRadius(5);
		sphereSrc->Update();
		auto sphere = sphereSrc->GetOutput();
		auto spmapper = vtkSmartPointer<vtkPolyDataMapper>::New();
		spmapper->SetInputData(sphere);
		auto spActor = vtkSmartPointer<vtkActor>::New();
		spActor->SetMapper(spmapper);
		spActor->SetPosition(points[i]);
		renderer->AddActor(spActor);
	}


	

	//renderer2->AddActor(actor1);
	//renderer2->AddActor(actor3);

	renWin->Render();
	interactor->Start();

	return 0;
}












