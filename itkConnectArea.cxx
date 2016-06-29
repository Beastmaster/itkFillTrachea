/*
Author: QIN Shuo
Date: 2016/6/28
Description:
	1. use ITK to read a nifti file
	2. Re-orient it to RAI direction (no sepcification, just the same)
	3. Select the bottom slice. Make the bottom slice black
	3. Select the bottom 2nd slice and fill holes
	The process is rather fast because we just process 1 slice
*/




#include <stdio.h>
#include <string>

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




int main1(int argc, char** argv)
{
	const unsigned int Dimension = 3;
	typedef unsigned char							PixelType;
	typedef itk::Image<unsigned char, Dimension>	ImageType;
	typedef itk::Image<unsigned char, Dimension-1>	Image2DType;
	
	std::string dir = "";
	std::string nifti_name = "E:/test/reference_brain_res.nii";
	
	// 1. read images
	// parse file name suffix and select reader
	typedef itk::ImageFileReader<ImageType>	   NiftiReaderType;
	auto niftiReader = NiftiReaderType::New();
	niftiReader->SetFileName(nifti_name);
	niftiReader->Update();
	/*
	// ===============================
	unsigned char lowerThresholdx = 10;
	unsigned char upperThresholdx = 255;
	typedef itk::BinaryThresholdImageFilter <ImageType, ImageType>
		BinaryThresholdImageFilterTypex;

	BinaryThresholdImageFilterTypex::Pointer thresholdFilterx
		= BinaryThresholdImageFilterTypex::New();
	thresholdFilterx->SetInput(niftiReader->GetOutput());
	thresholdFilterx->SetLowerThreshold(lowerThresholdx);
	thresholdFilterx->SetUpperThreshold(upperThresholdx);
	thresholdFilterx->SetInsideValue(255);
	thresholdFilterx->SetOutsideValue(0);
	try
	{
		std::cout << "Thresholding..." << std::endl;
		thresholdFilterx->Update();
	}
	catch (itk::ExceptionObject& e)
	{
		std::cout << "Thresholding process error!" << std::endl;
		std::cout << e;
		return 1;
	}
	// write to image
	typedef itk::ImageFileWriter<ImageType> NiftiWriterTypex;
	auto niftiWriterx = NiftiWriterTypex::New();
	niftiWriterx->SetFileName("E:/test/reference_brain_output.nii");
	niftiWriterx->SetInput(thresholdFilterx->GetOutput());
	try
	{
		std::cout << "Writing to file" << std::endl;
		niftiWriterx->Update();
	}
	catch (itk::ExceptionObject& e)
	{
		std::cout << "Writing to file process error!" << std::endl;
		std::cout << e;
	}
	exit(0);
	//==================================
	*/

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
		return 1;
	}

	auto Image = reOrientor->GetOutput();

	// Get Image Size
	ImageType::RegionType image_region = Image->GetLargestPossibleRegion();
	ImageType::SizeType image_size = image_region.GetSize();
	auto image_index = image_region.GetIndex();
	std::cout << "Image Size is: " << image_size << std::endl;
	std::cout << "Image Index is: " << image_index << std::endl;

	// extract region: bottom slices
	typedef	itk::ImageRegion<Dimension> RegionType;
	RegionType::SizeType size;
	RegionType::IndexType index;
	size[0] = image_size[0];
	size[1] = image_size[1];
	size[2] = 2;//image_size[2];
	index[0] = image_index[0];
	index[1] = image_index[1];
	index[2] = image_index[2]+1;
	RegionType region(index, size);

	// create a 2d slice
	auto ImageSlice = Image2DType::New();
	typedef	itk::ImageRegion<Dimension-1> Region2DType;
	Region2DType::SizeType size2d;
	Region2DType::IndexType index2d;
	size2d[0] = size[0];
	size2d[1] = size[1];
	index2d[0] = index[0];
	index2d[1] = index[1];
	Region2DType region2d(index2d,size2d);
	ImageSlice->SetRegions(region2d);
	ImageSlice->Allocate();
	

	// make the bottom slice black
	for (size_t idx0 = image_index[0]; idx0 < image_size[0]; idx0++)
	{
		for (size_t idx1 = image_index[1]; idx1 < image_size[1]; idx1++)
		{
			for (size_t idx2 = image_index[2]; idx2 < 1; idx2++)
			{
				ImageType::IndexType idx;
				Image2DType::IndexType idxx;
				idx[0] = idx0;
				idx[1] = idx1;
				idx[2] = idx2;
				idxx[0] = idx0;
				idxx[1] = idx1;
				Image->SetPixel(idx, 0);
				//ImageSlice->SetPixel(idxx, Image->GetPixel(idx));
			}
		}
	}


	std::cout << "Selecting slice..." << std::endl;
	for (size_t idx0 = index[0]; idx0 < size[0]; idx0++)
	{
		for (size_t idx1 = index[1]; idx1 < size[1]; idx1++)
		{
			for (size_t idx2 = index[2]; idx2 < size[2]; idx2++)
			{
				ImageType::IndexType idx;
				Image2DType::IndexType idxx;
				idx[0] = idx0;
				idx[1] = idx1;
				idx[2] = idx2;
				idxx[0] = idx0;
				idxx[1] = idx1;
				//Image->SetPixel(idx, ImageSlice->GetPixel(idxx));
				ImageSlice->SetPixel(idxx,Image->GetPixel(idx));
			}
		}
	}
	//typedef itk::ExtractImageFilter< ImageType, Image2DType > ExtractFilterType;
	//auto extractFilter = ExtractFilterType::New();
	//extractFilter->SetExtractionRegion(region);
	//extractFilter->SetInput(Image);
	//extractFilter->SetDirectionCollapseToIdentity();
	//try
	//{
	//	extractFilter->Update();
	//}
	//catch (itk::ExceptionObject& e)
	//{
	//	std::cout << "Extract process error!" << std::endl;
	//	std::cout << e;
	//	return 1;
	//}


	// 3. Threshold to binary image
	unsigned char lowerThreshold = 10;
	unsigned char upperThreshold = 255;
	typedef itk::BinaryThresholdImageFilter <Image2DType, Image2DType>
		BinaryThresholdImageFilterType;

	BinaryThresholdImageFilterType::Pointer thresholdFilter
		= BinaryThresholdImageFilterType::New();
	thresholdFilter->SetInput(ImageSlice);
	//thresholdFilter->SetInput(niftiReader->GetOutput());
	thresholdFilter->SetLowerThreshold(lowerThreshold);
	thresholdFilter->SetUpperThreshold(upperThreshold);
	thresholdFilter->SetInsideValue(255);
	thresholdFilter->SetOutsideValue(0);
	try
	{
		std::cout << "Thresholding..." << std::endl;
		thresholdFilter->Update();
	}
	catch (itk::ExceptionObject& e)
	{
		std::cout << "Thresholding process error!" << std::endl;
		std::cout << e;
		return 1;
	}


	//// 4.1 Run fill hold algorithm
	//typedef itk::VotingBinaryHoleFillingImageFilter< Image2DType, Image2DType > FillingHoleFilter;
	//FillingHoleFilter::Pointer fillingHoleFilter = FillingHoleFilter::New();
	//ImageType::SizeType indexRadius;
	//indexRadius[0] = 100; // along x
	//indexRadius[1] = 100; // along Y
	////indexRadius[2] = 2; // along Z
	//fillingHoleFilter->SetInput(thresholdFilter->GetOutput());
	//fillingHoleFilter->SetBackgroundValue(0);
	//fillingHoleFilter->SetForegroundValue(255);
	//try
	//{
	//	std::cout << "Voting binary hole filling ..." << std::endl;
	//	fillingHoleFilter->Update();
	//}
	//catch (itk::ExceptionObject& e)
	//{
	//	std::cout << "Filling Holes process error!" << std::endl;
	//	std::cout << e;
	//	return 1;
	//}

	// 4.3 Run Connected algorithm ==   test
	typedef itk::BinaryFillholeImageFilter< Image2DType > FillHoleType;
	auto fillHoleFilter = FillHoleType::New();
	fillHoleFilter->SetInput(thresholdFilter->GetOutput());
	//fillHoleFilter->SetInput(thresholdFilter->GetOutput());
	fillHoleFilter->SetFullyConnected(1);
	fillHoleFilter->SetForegroundValue(itk::NumericTraits < PixelType > ::max());
	try
	{
		std::cout << "Filling Holes ..." << std::endl;
		fillHoleFilter->Update();
	}
	catch (itk::ExceptionObject& e)
	{
		std::cout << "Filling Holes process error!" << std::endl;
		std::cout << e;
		return 1;
	}

	ImageSlice = fillHoleFilter->GetOutput();

	// 4.2 Replace region
	std::cout << "Putting slice back to image ..." << std::endl;
	for (size_t idx0 = index[0]; idx0 < size[0]; idx0++)
	{
		for (size_t idx1 = index[1]; idx1 < size[1]; idx1++)
		{
			for (size_t idx2 = index[2]; idx2 < size[2]; idx2++)
			{
				ImageType::IndexType idx;
				Image2DType::IndexType idxx;
				idx[0] = idx0;
				idx[1] = idx1;
				idx[2] = idx2;
				idxx[0] = idx0;
				idxx[1] = idx1;
				Image->SetPixel(idx, ImageSlice->GetPixel(idxx));
				//Image->SetPixel(idx,255 );
			}
		}
	}

	// write to image
	typedef itk::ImageFileWriter<ImageType> NiftiWriterType;;
	auto niftiWriter = NiftiWriterType::New();
	niftiWriter->SetFileName("E:/test/reference_brain_output.nii");
	niftiWriter->SetInput(Image);
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


