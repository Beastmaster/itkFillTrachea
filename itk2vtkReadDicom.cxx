/*
Author: QIN Shuo
Date: 2016/6/28
Description:
	Use itk-gdcm to read a dicom series
	Test itk-vtk bridge function
	Test registration function
*/

#include "itkImageSeriesReader.h"
#include "itkImageSeriesWriter.h"
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"







int mainxx(int argc, char** argv)
{
	const unsigned int Dimension = 3;
	typedef unsigned char							PixelType;
	typedef itk::Image<unsigned char, Dimension>	ImageType;
	typedef itk::ImageSeriesReader< ImageType >		DicomReaderType;
	typedef itk::GDCMImageIO						DicomImageIOType;
	typedef itk::GDCMSeriesFileNames				NamesGeneratorType;











	return 0;
}






