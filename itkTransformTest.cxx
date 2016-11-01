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
#include <stdlib.h>
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
#include "itkResampleImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkPointSet.h"

#include <vtkSmartPointer.h>
#include <vtkTransform.h>
#include <vtkMatrix4x4.h>
#include <vtkNIFTIImageReader.h>
#include <vtkMarchingCubes.h>
#include <vtkPolyDataConnectivityFilter.h>
#include <vtkConnectivityFilter.h>
#include <vtkGeometryFilter.h>

#include <vtkMatrix4x4.h>
#include <vtkTransform.h>




/*          #### NOt tested yet
Input:
	list	: A list of 3d point tobe transformed. The coordinate are ITK system
	filename: The file generated from itkRegistrationTest function.
Note:
	the list is reference, the value will be modified without return.
*/
int itkTransform(std::vector<double*>& list , std::string filename)
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


		reader->SetFileName(filename.c_str());
		reader->Update();
		reader->GetTransformList();

		auto list = reader->GetTransformList();
		auto transform = *(list->begin());
		return transform;
	};
	
	auto tt = read_transform(filename);//("E:/WorkPlace/Cpp/vtk_solutions/TMS/build/Release/temp/transformAffine.txt");

	tt->Print(std::cout);

	//typedef itk::AffineTransform <double, 3> TransformType;
	typedef itk::MatrixOffsetTransformBase<double, 3> TransformType;

	auto transform = TransformType::New();
	transform->SetIdentity();
	transform->SetParameters(tt->GetParameters());
	transform->SetFixedParameters(tt->GetFixedParameters());
	transform->Print(std::cout);
	
	auto n_transform = TransformType::New();
	n_transform->UpdateTransformParameters(transform->GetInverseTransform()->GetParameters());

	auto matrix = transform->GetInverseMatrix();
	auto center = transform->GetCenter();
	auto offset = transform->GetOffset();
	auto translation = transform->GetTranslation();

	auto reverse = transform->GetInverseTransform();
	typedef float PixelType;
    const unsigned int Dimension = 3;
    typedef itk::PointSet< PixelType, Dimension >   PointSetType;
    typedef PointSetType::PointType PointType;
	for (auto it = list.begin(); it != list.end(); ++it)
	{
		transform->TransformPoint((*it));
	}
	return 0;
}



vtkSmartPointer<vtkMatrix4x4> readITKTransform(std::string filename)
{
	auto read_transform = [](std::string filename)
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


		reader->SetFileName(filename.c_str());
		reader->Update();
		reader->GetTransformList();

		auto list = reader->GetTransformList();
		auto transform = *(list->begin());
		return transform;
	};

	auto tt = read_transform(filename);

	tt->Print(std::cout);

	//typedef itk::AffineTransform <double, 3> TransformType;
	typedef itk::MatrixOffsetTransformBase<double, 3> TransformType;

	//typedef itk::CompositeTransform< double, 3 > CompositeTransformType;
	//CompositeTransformType::Pointer composite = CompositeTransformType::New();

	auto transform = TransformType::New();
	transform->SetIdentity();
	transform->SetParameters(tt->GetParameters());
	transform->SetFixedParameters(tt->GetFixedParameters());
	transform->Print(std::cout);
	
	auto n_transform = TransformType::New();
	n_transform->UpdateTransformParameters(transform->GetInverseTransform()->GetParameters());

	auto matrix = transform->GetInverseMatrix();
	auto center = transform->GetCenter();
	auto offset = transform->GetOffset();
	auto translation = transform->GetTranslation();


	auto output_matrix = vtkSmartPointer<vtkMatrix4x4>::New();
	output_matrix->Identity();
	
	output_matrix->SetElement(0, 0, matrix[0][0]);
	output_matrix->SetElement(1, 0, matrix[1][0]);
	output_matrix->SetElement(2, 0, matrix[2][0]);
	
	output_matrix->SetElement(0, 1, matrix[0][1]);
	output_matrix->SetElement(1, 1, matrix[1][1]);
	output_matrix->SetElement(2, 1, matrix[2][1]);

	output_matrix->SetElement(0, 2, matrix[0][2]);
	output_matrix->SetElement(1, 2, matrix[1][2]);
	output_matrix->SetElement(2, 2, matrix[2][2]);

	output_matrix->SetElement(0, 3, -offset[0]);
	output_matrix->SetElement(1, 3, -offset[1]);
	output_matrix->SetElement(2, 3, -offset[2]);

	return output_matrix;
}





int itkTransformImage(std::string transform_file, std::string in_filename, std::string out_filename)
{
	auto read_transform = [](std::string filename)
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


		reader->SetFileName(filename.c_str());
		reader->Update();
		reader->GetTransformList();

		auto list = reader->GetTransformList();
		auto transform = *(list->begin());
		return transform;
	};

	auto tt = read_transform(transform_file);

	typedef itk::AffineTransform <double, 3> TransformType;
	auto transform = TransformType::New();
	transform->SetParameters(tt->GetParameters());
	transform->SetFixedParameters(tt->GetFixedParameters());
//	transform->UpdateTransformParameters();

	typedef itk::Image<int, 3> ImageType;
	typedef itk::ResampleImageFilter<ImageType,ImageType> ResampleType;

	typedef itk::ImageFileReader<ImageType> ReaderType;
	auto reader = ReaderType::New();
	reader->SetFileName(in_filename);
	reader->Update();
	auto ori_img = reader->GetOutput();

	auto write = [](std::string filename,ImageType::Pointer img)
	{
		typedef itk::ImageFileWriter<ImageType> WriterType;
		auto writer = WriterType::New();
		writer->SetInput(img);
		writer->SetFileName(filename);
		writer->Update();
	};

	auto resampler = ResampleType::New();
	resampler->SetInput(ori_img);
	resampler->SetSize(ori_img->GetLargestPossibleRegion().GetSize());
	resampler->SetOutputOrigin(ori_img->GetOrigin());
	resampler->SetOutputSpacing(ori_img->GetSpacing());
	resampler->SetOutputDirection(ori_img->GetDirection());
	resampler->SetTransform(transform);
	resampler->UseReferenceImageOn();
	resampler->Update();
	write(out_filename,resampler->GetOutput());

	return 0;
}





