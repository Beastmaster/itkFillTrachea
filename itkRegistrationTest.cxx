/*
Author: QIN Shuo
Date: 2016/6/28
Description:
	Test registration function
	In this version, I use affine transform instead of rigidbody transform
	
	Transform point from with the affine transform we get
*/

#include <string.h>
#include <stdio.h>
#include "itkImage.h"
#include "itkImageFileWriter.h"


#include "itkOrientImageFilter.h"
#include "itkImageRegion.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkExtractImageFilter.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkBinaryFillholeImageFilter.h"
#include "itkVotingBinaryHoleFillingImageFilter.h"




#include "Register.h"




int main(int argc, char** argv)
{
	const unsigned int Dimension = 3;
	typedef unsigned char							PixelType;
	typedef itk::Image<unsigned char, Dimension>	ImageType;
	typedef itk::Image<unsigned char, Dimension - 1>	Image2DType;

	//std::string fixed_name = argv[1];
	//std::string moving_name = argv[2];
	std::string fixed_name = "E:/test/ch2.nii";
	std::string moving_name = "E:/test/reference_brain_res.nii";
	std::string out_name = "E:/test/reference_brain_reg.nii";

	// 1. read images
	auto read_nifti = [](std::string name, ImageType::Pointer & img)
	{
		typedef itk::ImageFileReader<ImageType>	   NiftiReaderType;
		typedef itk::OrientImageFilter<ImageType, ImageType> ReOrientorType;
		auto reader = NiftiReaderType::New();
		reader->SetFileName(name);
		reader->Update();
		auto reOrientor = ReOrientorType::New();
		reOrientor->UseImageDirectionOn();
		reOrientor->SetDesiredCoordinateOrientation(itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RAI);
		reOrientor->SetInput(reader->GetOutput());
		reOrientor->Update();
		img = reOrientor->GetOutput();
	};
	//auto reader2 = NiftiReaderType::New();
	//reader2->SetFileName(moving_name);
	//reader2->Update();
	//auto reOrientor2 = ReOrientorType::New();
	//reOrientor2->UseImageDirectionOn();
	//reOrientor2->SetDesiredCoordinateOrientation(itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RAI);
	//reOrientor2->SetInput(reader2->GetOutput());
	//auto moving_image = reOrientor2->GetOutput();


	auto fix_image = ImageType::New();
	auto moving_image = ImageType::New();
	read_nifti(fixed_name,fix_image);
	read_nifti(moving_name, moving_image);
	
	auto output_image = ImageType::New();

	typedef Register< PixelType, PixelType, PixelType > RegisterType_process;
	RegisterType_process * reg_hd = new RegisterType_process;
	reg_hd->SetFixedImage(fix_image);
	reg_hd->SetMovingImage(moving_image);
	reg_hd->GenerateTranformMatrix();
	reg_hd->GetRegisteredMovingImage(output_image);
	delete reg_hd;

	// write to image
	typedef itk::ImageFileWriter<ImageType> NiftiWriterType;;
	auto niftiWriter = NiftiWriterType::New();
	niftiWriter->SetFileName(out_name);
	niftiWriter->SetInput(output_image);
	try
	{
		std::cout << "Writing to file" << std::endl;
		niftiWriter->Update();
	}
	catch (itk::ExceptionObject& e)
	{
		std::cout << "Writing to file process error!" << std::endl;
		std::cout << e;
	}

	return 0;
}






