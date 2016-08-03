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


int main(int argc, char** argv)
{

	std::string dicomdir = "E:/test/head-dicom";
	std::string niftiname = "E:/test/head.nii";

	// read dicom files
	auto itk_img1 = itk2vtkReadDicom(dicomdir.c_str());
	// write to nifti file
	typedef itk::ImageFileWriter< itk::Image<int,3> > WriterType;
	WriterType::Pointer writer = WriterType::New();
	std::string outFileName;
	outFileName = "E:/test/head.nii";
	writer->SetFileName(outFileName);
	writer->UseCompressionOn();
	writer->SetInput(itk_img1);
	std::cout << "Writing: " << outFileName << std::endl;
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
	double pp[][3] = 
	{ 	{-72.2834,1.5069,-57.318 },
		{-48.982,-74.945,-25.526},
		{5.2913,-109.121,-58.876},
		{59.924,-76.659,-25.647},
		{87.947,-0.1983,-60.755} };

	for (size_t i = 0; i < 5; i++)
	{
		double * temp = new double(3);

		// transform to itk coordiante
		//FindVTKCoordinateInITK(moving.c_str(), pp[i], temp);

		list.push_back(pp[i]);
	}

	double pp2[][3]
	{
		{115.154, 209.358, 33.1324 },
		{ 53.4593, 171.363, 69.5877, },
		{ 27.4593, 199.137, 122.67 },
		{ 136.621, 211.747, 195.237 },
		{ 67.202, 172.043, 174.883 }
	};
	std::vector<double*> list2;
	for (size_t i = 0; i < 5; i++)
	{
		double *temp = new double(3);
		memcpy(temp,list[i],sizeof(double)*3);
		list2.push_back(temp);
		//list2.push_back(pp2[i]);
	}


	itkTransform(list2, preffix+"Affine.txt");

	std::cout << "List:" << std::endl;
	for (auto it = list.begin(); it != list.end(); ++it)
	{
		for (int i = 0; i < 3; i++)
			std::cout << (*it)[i] << ",";
	}
	std::cout << std::endl << "List2:" << std::endl;
	for (auto it = list2.begin(); it != list2.end(); ++it)
	{
		for (int i = 0; i < 3; i++)
			std::cout << (*it)[i] << ",";
	}

	// fill hole
	std::string filled_name = "E:/test/filled_skin.nii";  // use this file to extract skin surface
	//fillHoleFilter(fix, warp_brain, filled_name,50);

	//run marchingcubes
	typedef itk::ImageFileReader<itk::Image<int, 3> > ITKReaderType;
	auto itkreader1 = ITKReaderType::New();
	itkreader1->SetFileName(moving); itkreader1->Update();
	auto vtk_img = itk2vtkCoordinate(itkreader1->GetOutput());
	auto suf = Marching_Connected(vtk_img, 10);


	auto itkreader2 = ITKReaderType::New();
	itkreader2->SetFileName(fix); itkreader2->Update();
	auto vtk_img2 = itk2vtkCoordinate(itkreader2->GetOutput());
	auto suf2 = Marching_Connected(vtk_img2, 50);


	auto write_stl = [](vtkSmartPointer<vtkPolyData> suf, std::string filename)
	{
		std::cout << "Writing to  " << filename << std::endl;
		auto stl_writer = vtkSmartPointer< vtkSTLWriter>::New();
		stl_writer->SetInputData(suf);
		stl_writer->SetFileName(filename.c_str());
		stl_writer->Update();
	};

	//write_stl(suf,"E:/test/fix.stl");
	write_stl(suf2, "E:/test/resampled.stl");


	auto create_ball_actors = [](std::vector<double*> list)
	{
		// ball
		auto createBall = []()
		{
			auto ball = vtkSmartPointer<vtkSphereSource>::New();
			ball->SetCenter(0.0, 0.0, 0.0);
			ball->SetThetaResolution(50);
			ball->SetRadius(5.0);
			ball->Update();
			auto ballMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
			ballMapper->SetInputData(ball->GetOutput());
			auto ballActor = vtkSmartPointer<vtkActor>::New();
			ballActor->SetMapper(ballMapper);
			ballActor->GetProperty()->SetColor(1, 0, 0);
			return ballActor;
		};

		std::vector<vtkSmartPointer<vtkActor> > ball1;
		for (auto it = list.begin(); it != list.end(); ++it)
		{
			auto bb = createBall();
			bb->SetPosition(*it);
			ball1.push_back(bb);
		}
		return ball1;
	};

	auto ball_list1 = create_ball_actors(list); 
	auto ball_list2 = create_ball_actors(list2);


	//skin actor
	auto skin_mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	skin_mapper->SetInputData(suf);
	auto skin_actor = vtkSmartPointer<vtkActor>::New();
	skin_actor->SetMapper(skin_mapper);



	//renderer1
	vtkSmartPointer<vtkRenderer> renderer =
		vtkSmartPointer<vtkRenderer>::New();
	renderer->AddActor(skin_actor);
	for (auto it = ball_list1.begin(); it != ball_list1.end(); ++it)
	{
		renderer->AddActor((*it));
	}
	//renderer->AddActor(skin_actorx);

	// skin 2
	auto skin_mapper2 = vtkSmartPointer<vtkPolyDataMapper>::New();
	skin_mapper2->SetInputData(suf2);
	auto skin_actor2 = vtkSmartPointer<vtkActor>::New();
	skin_actor2->SetMapper(skin_mapper2);
	// renderer2
	vtkSmartPointer<vtkRenderer> renderer2 =
		vtkSmartPointer<vtkRenderer>::New();
	renderer2->AddActor(skin_actor2);
	for (auto it = ball_list2.begin(); it != ball_list2.end(); ++it)
	{
		renderer2->AddActor((*it));
	}


	vtkSmartPointer<vtkRenderWindow> renwin =
		vtkSmartPointer<vtkRenderWindow>::New();
	renwin->AddRenderer(renderer);
	renderer->SetViewport(0,0,0.5,1);

	renwin->AddRenderer(renderer2);
	renderer2->SetViewport(0.5, 0, 1,1);

	vtkSmartPointer<vtkRenderWindowInteractor> iren =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();
	iren->SetRenderWindow(renwin);
	iren->Initialize();
	iren->Start();


	return 0;
}












