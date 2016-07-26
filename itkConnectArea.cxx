/*
Author: QIN Shuo
Date: 2016/6/28
Description:
	1. use ITK to read a nifti file
	2. Re-orient it to RAI direction (no sepcification, just the same)
	3. Select the bottom slice. Make the bottom slice black
	3. Select the bottom 2nd slice and fill holes
	4. Use the brain to elminate some holes
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

#include "itkFlatStructuringElement.h"
#include "itkBinaryDilateImageFilter.h"
#include "itkOrImageFilter.h"
#include "itkChangeInformationImageFilter.h"




/*
Input:
	ori_name : Orignal image file name.
	brain	 : The extracted brain of original image
	filled	 : Hole filling result. A thresholded image
	threshold: int type threshold, lower end.
*/
int fillHoleFilter(std::string ori_name,std::string brain, std::string filled,int threshold)
{
	int lowerThreshold = threshold;// 10;
	int upperThreshold = 10000;


	const unsigned int Dimension = 3;
	typedef  int							PixelType;
	typedef itk::Image< int, Dimension>		ImageType;
	typedef itk::Image< int, Dimension-1>	Image2DType;
	
	std::string nifti_name = ori_name;//"E:/test/reference_brain_res.nii";
	
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
		//reOrientor->Update();
	}
	catch ( itk::ExceptionObject& e)
	{
		std::cout << "Re-Orient process error!" << std::endl;
		std::cout << e;
		return 1;
	}
	auto Image = niftiReader->GetOutput();


	// 3. Threshold to binary image
	typedef itk::BinaryThresholdImageFilter <ImageType, ImageType>
		BinaryThresholdImageFilterType;

	auto thresholdFilter = BinaryThresholdImageFilterType::New();
	thresholdFilter->SetInput(Image);
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
	Image = thresholdFilter->GetOutput();


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
	index[2] = image_index[2]+1;  //??
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



	// 4.3 Run Connected algorithm
	typedef itk::BinaryFillholeImageFilter< Image2DType > FillHoleType;
	auto fillHoleFilter = FillHoleType::New();
	fillHoleFilter->SetInput(ImageSlice);
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

	//=== Read brain, run image dilate and  OR
	std::cout << "Running Dilation ...."<<std::endl;
	auto brain_reader = NiftiReaderType::New();
	brain_reader->SetFileName(brain);// ("E:/test/reference_brain_aal.nii");
	brain_reader->Update();
	auto reOrientor2 = ReOrientorType::New();
	reOrientor2->UseImageDirectionOn();
	reOrientor2->SetDesiredCoordinateOrientation(itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RAI);
	reOrientor2->SetInput(brain_reader->GetOutput());
	//reOrientor2->Update();
	lowerThreshold = 1;   // brain value are small
	upperThreshold = 255; // brain value are small
	typedef itk::BinaryThresholdImageFilter <ImageType, ImageType>
		BinaryThresholdImageFilterType;
	auto thresholdbrainFilter = BinaryThresholdImageFilterType::New();
	thresholdbrainFilter->SetInput(brain_reader->GetOutput());
	thresholdbrainFilter->SetLowerThreshold(lowerThreshold);
	thresholdbrainFilter->SetUpperThreshold(upperThreshold);
	thresholdbrainFilter->SetInsideValue(255);
	thresholdbrainFilter->SetOutsideValue(0);
	thresholdbrainFilter->Update();

	// image dilate
	typedef itk::FlatStructuringElement< Dimension >
		StructuringElementType;
	StructuringElementType::RadiusType radius;
	radius.Fill(10.0);
	StructuringElementType structuringElement =
		StructuringElementType::Ball(radius);

	typedef itk::BinaryDilateImageFilter< ImageType, ImageType,
		StructuringElementType > BinaryDilateImageFilterType;

	BinaryDilateImageFilterType::Pointer dilateFilter =
		BinaryDilateImageFilterType::New();
	dilateFilter->SetInput(thresholdbrainFilter->GetOutput());
	dilateFilter->SetKernel(structuringElement);
	dilateFilter->Update();

	//== Threshold and run OR operator
	std::cout << "Running OR operator ...." << std::endl;
	lowerThreshold = 10;
	upperThreshold = 255;
	auto thresholdheadFilter = BinaryThresholdImageFilterType::New();
	thresholdheadFilter->SetInput(Image);
	thresholdheadFilter->SetLowerThreshold(lowerThreshold);
	thresholdheadFilter->SetUpperThreshold(upperThreshold);
	thresholdheadFilter->SetInsideValue(255);
	thresholdheadFilter->SetOutsideValue(0);
	thresholdheadFilter->Update();
	typedef itk::OrImageFilter <ImageType,ImageType,ImageType> OrImageFilterType;
	OrImageFilterType::Pointer orFilter = OrImageFilterType::New();
	orFilter->SetInput1(thresholdheadFilter->GetOutput());
	orFilter->SetInput2(dilateFilter->GetOutput());
	orFilter->SetCoordinateTolerance(0.01);
	try
	{
		std::cout << "Running OR operation" << std::endl;
		orFilter->Update();
	}
	catch (itk::ExceptionObject& e)
	{
		std::cout << "Running OR operation process error!" << std::endl;
		std::cout << e;
	}

	// save
	auto writer = itk::ImageFileWriter<ImageType>::New();
	writer->SetFileName(filled);
	writer->SetInput(orFilter->GetOutput());
	try
	{
		std::cout << "Writing to file" << std::endl;
		writer->Update();
	}
	catch (itk::ExceptionObject& e)
	{
		std::cout << "Writing to file process error!" << std::endl;
		std::cout << e;
	}


	return 0;
}


