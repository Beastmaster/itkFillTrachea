

#include "itkImage.h"
#include "itkImageFileWriter.h"

#include <stdio.h>
#include <string>
#include "itkImageSeriesReader.h"
#include "itkImageSeriesWriter.h"
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkOrientImageFilter.h"
#include "itkImageRegion.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkExtractImageFilter.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkBinaryFillholeImageFilter.h"
#include "itkVotingBinaryHoleFillingImageFilter.h"





int main(int argc, char** argv)
{
	const unsigned int Dimension = 3;
	typedef itk::Image<unsigned char, Dimension>	ImageType;
	typedef itk::ImageSeriesReader< ImageType >		DicomReaderType;
	typedef itk::GDCMImageIO						DicomImageIOType;
	typedef itk::GDCMSeriesFileNames				NamesGeneratorType;
	
	std::string dir = "";
	std::string nifti_name = "";
	
	// 1. read images
	// parse file name suffix and select reader
	typedef itk::ImageFileReader<ImageType>	   NiftiReaderType;
	auto niftiReader = NiftiReaderType::New();
	niftiReader->SetFileName(nifti_name);
	niftiReader->Update();

	// 2. re-orient images
	typedef itk::OrientImageFilter<ImageType, ImageType> ReOrientorType;
	auto reOrientor = ReOrientorType::New();
	reOrientor->UseImageDirectionOn();
	reOrientor->SetDesiredCoordinateOrientation(itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RAI);
	reOrientor->SetInput(niftiReader->GetOutput());
	try
	{
		reOrientor->Update();
	}
	catch ( itk::ExceptionObject& e)
	{
		std::cout << "Re-Orient process error!" << std::endl;
		std::cout << e;
	}


	// extract region: bottom slices
	typedef	itk::ImageRegion<Dimension> RegionType;
	RegionType::SizeType size;
	RegionType::IndexType index;
	//size[0] = 1;
	//size[1] = 1;
	//size[2] = 1;
	//index[0] = 1;
	//index[1] = 1;
	//index[2] = 1;
	RegionType region(index, size);

	typedef itk::ExtractImageFilter< ImageType, ImageType > ExtractFilterType;
	auto extractFilter = ExtractFilterType::New();
	extractFilter->SetExtractionRegion(region);
	extractFilter->SetInput(reOrientor->GetOutput());
	extractFilter->SetDirectionCollapseToIdentity();
	try
	{
		extractFilter->Update();
	}
	catch (itk::ExceptionObject& e)
	{
		std::cout << "Extract process error!" << std::endl;
		std::cout << e;
	}


	// 3. Threshold to binary image
	unsigned char lowerThreshold = 10;
	unsigned char upperThreshold = 11;
	typedef itk::BinaryThresholdImageFilter <ImageType, ImageType>
		BinaryThresholdImageFilterType;

	BinaryThresholdImageFilterType::Pointer thresholdFilter
		= BinaryThresholdImageFilterType::New();
	thresholdFilter->SetInput(extractFilter->GetOutput());
	thresholdFilter->SetLowerThreshold(lowerThreshold);
	thresholdFilter->SetUpperThreshold(upperThreshold);
	thresholdFilter->SetInsideValue(255);
	thresholdFilter->SetOutsideValue(0);
	try
	{
		thresholdFilter->Update();
	}
	catch (itk::ExceptionObject& e)
	{
		std::cout << "Thresholding process error!" << std::endl;
		std::cout << e;
	}


	// 4.1 Run fill hold algorithm
	typedef itk::VotingBinaryHoleFillingImageFilter< ImageType, ImageType > FillingHoleFilter;
	FillingHoleFilter::Pointer fillingHoleFilter = FillingHoleFilter::New();
	ImageType::SizeType indexRadius;
	indexRadius[0] = 0; // along x
	indexRadius[1] = 0; // along Y
	indexRadius[2] = 0; // along Z
	fillingHoleFilter->SetInput(thresholdFilter->GetOutput());
	fillingHoleFilter->SetBackgroundValue(0);
	fillingHoleFilter->SetForegroundValue(255);
	try
	{
		fillingHoleFilter->Update();
	}
	catch (itk::ExceptionObject& e)
	{
		std::cout << "Thresholding process error!" << std::endl;
		std::cout << e;
	}

	// 4.2 Replace region
//	image->SetPixel(pixelIndex, 128);


	// 4.3 Run Connected algorithm ==   test

	// write to image


	return 0;
}


