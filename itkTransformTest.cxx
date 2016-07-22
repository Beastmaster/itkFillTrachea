/*
Author: QIN Shuo
Date: 2016/7/11
Description:
	Read the affine transform buf txt
	Construct a itkAffineTransform
	Convert itk transform to an vtk transform matrix
*/


#include <string>
#include <functional>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <fstream>
#include "itkAffineTransform.h"
#include "itkPoint.h"
#include "itkTransformMeshFilter.h"
// Read itk transform
#include "itkTransformFileReader.h"
#include "itkTransformFactoryBase.h"
#include "itkTransformFactory.h"
#include "itkMatrixOffsetTransformBase.h"


#include <vtkSmartPointer.h>
#include <vtkTransform.h>
#include <vtkMatrix4x4.h>
#include <vtkNIFTIImageReader.h>
#include <vtkMarchingCubes.h>
#include <vtkPolyDataConnectivityFilter.h>
#include <vtkConnectivityFilter.h>
#include <vtkGeometryFilter.h>

#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSphereSource.h>
#include <vtkProperty.h>
#include <vtkImageData.h>
#include <vtkTransform.h>





int itkTransform(std::vector<double*> list)
{
	
	auto read_transform = []( std::string filename)
	{
		// Register default transforms
		// itk::TransformFactoryBase::RegisterDefaultTransforms();
		typedef itk::MatrixOffsetTransformBase< double, 3, 3 > MatrixOffsetTransformType;
		itk::TransformFactory<MatrixOffsetTransformType>::RegisterTransform();

		itk::TransformFileReaderTemplate<double>::Pointer reader =
		itk::TransformFileReaderTemplate<double>::New();

		// replace keywords: MatrixOffsetTransformBase -->> AffineTransform
		auto replace_keywords = [filename]()
		{
			std::ifstream t(filename);
			std::stringstream buffer;
			buffer << t.rdbuf();
			auto content = buffer.str();

			std::string substr = "MatrixOffsetTransformBase";
			std::string newsub = "AffineTransform";

			// replace substr with newsub
			std::size_t found = content.find(substr);
			if (found == std::string::npos)
			{
				std::cout << "No need to replace" << std::endl;
				return;
			}
			content.replace(content.find(substr), substr.length(), newsub);
			std::cout << content << '\n';

			std::ofstream ww(filename);
			ww << content;
			ww.flush();
		};

		//replace_keywords();

		reader->SetFileName(filename.c_str());
		reader->Update();
		reader->GetTransformList();

		auto list = reader->GetTransformList();
		auto transform = *(list->begin());
		return transform;
	};
	
	auto tt = read_transform("E:/WorkPlace/Cpp/vtk_solutions/TMS/build/Release/temp/transformAffine.txt");

	tt->Print(std::cout);
	tt->GetTransformTypeAsString();

	
	typedef itk::AffineTransform <double, 3> TransformType;


	typedef itk::CompositeTransform< double, 3 > CompositeTransformType;
	CompositeTransformType::Pointer composite = CompositeTransformType::New();

	auto transform = TransformType::New();
	//transform->SetTranslation();

	return 0;
}



int Visulize()
{

	//=== vtk operation
	//ball actor
	auto ball = vtkSmartPointer<vtkSphereSource>::New();
	ball->SetCenter(0, 0, 0);
	ball->SetRadius(10);
	ball->SetThetaResolution(10);
	ball->Update();
	auto ball_mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	ball_mapper->SetInputData(ball->GetOutput());
	auto ball_actor = vtkSmartPointer<vtkActor>::New();
	ball_actor->SetMapper(ball_mapper);
	ball_actor->GetProperty()->SetColor(1.0, 0, 0);

	auto ball_mapper2 = vtkSmartPointer<vtkPolyDataMapper>::New();
	ball_mapper2->SetInputData(ball->GetOutput());
	auto ball_actor2 = vtkSmartPointer<vtkActor>::New();
	ball_actor2->SetMapper(ball_mapper2);
	ball_actor2->GetProperty()->SetColor(0.0, 1.0, 0);



	auto generate_skin = [](std::string file_name)
	{
		//std::string file_name = "E:/WorkPlace/Cpp/vtk_solutions/TMS/build/Release/configFile/Atlas/reference_brain_res.nii";
		auto reader = vtkNIFTIImageReader::New();
		reader->SetFileName(file_name.c_str());
		reader->Update();
		//auto img = reader->GetOutput();

		// marching...
		auto marchingCubes = vtkSmartPointer<vtkMarchingCubes>::New();
		marchingCubes->SetInputData(reader->GetOutput());
		//marchingCubes->SetNumberOfContours(2);
		marchingCubes->SetValue(0, 50);
		//marchingCubes update
		marchingCubes->Update();
		auto skin = marchingCubes->GetOutput();

		vtkSmartPointer<vtkPolyDataConnectivityFilter> connectivityFilter =
			vtkSmartPointer<vtkPolyDataConnectivityFilter>::New();
		connectivityFilter->SetInputData(skin);
		connectivityFilter->SetExtractionModeToLargestRegion();
		//connectivityFilter->Update();

		vtkSmartPointer<vtkGeometryFilter> geometryFilter =
			vtkSmartPointer<vtkGeometryFilter>::New();
		geometryFilter->SetInputData(skin);
		geometryFilter->Update();
		skin = geometryFilter->GetOutput();

		//skin actor
		auto skin_mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
		skin_mapper->SetInputData(skin);
		auto skin_actor = vtkSmartPointer<vtkActor>::New();
		skin_actor->SetMapper(skin_mapper);
		return skin_actor;
	};
	std::string file_name = "E:/WorkPlace/Cpp/vtk_solutions/TMS/build/Release/temp/data_reg.nii";
	std::string file_name2 = "E:/WorkPlace/Cpp/vtk_solutions/TMS/build/Release/configFile/Atlas/reference_brain_res.nii";

	auto skin_actor = generate_skin(file_name);
	auto skin_actor2 = generate_skin(file_name2);

	vtkSmartPointer<vtkRenderer> renderer =
		vtkSmartPointer<vtkRenderer>::New();
	skin_actor->GetProperty()->SetColor(1, 0.5, 1);
	//skin_actor->GetProperty()->SetOpacity(0.8);
	renderer->AddActor(skin_actor);
	skin_actor2->GetProperty()->SetColor(0.5, 1, 1);
	skin_actor2->GetProperty()->SetOpacity(0.8);
	renderer->AddActor(skin_actor2);
	renderer->AddActor(ball_actor);
	renderer->AddActor(ball_actor2);

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





