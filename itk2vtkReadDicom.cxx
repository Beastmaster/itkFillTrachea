/*
Author: QIN Shuo
Date: 2016/6/28
Description:
	Use itk-gdcm to read a dicom series
	Test itk-vtk bridge function
	
*/

#include "itkImage.h"
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkImageSeriesReader.h"
#include "itkImageFileWriter.h"
#include "itkOrientImageFilter.h"


/*
Input: dicom file dir
output: itk::Image<unsigned,3>
*/
itk::Image<  int, 3 >::Pointer itk2vtkReadDicom(const char*  dirname)
{

	typedef  int    PixelType;
	const  int      Dimension = 3;
	typedef itk::Image< PixelType, Dimension >         ImageType;
	typedef itk::ImageSeriesReader< ImageType >        ReaderType;
	
	std::string dirName = dirname;//"E:/test/head-dicom"; //current directory by default

	typedef itk::GDCMSeriesFileNames NamesGeneratorType;
	NamesGeneratorType::Pointer nameGenerator = NamesGeneratorType::New();
	nameGenerator->SetUseSeriesDetails(true);
	nameGenerator->AddSeriesRestriction("0008|0021");  // series date
	nameGenerator->SetGlobalWarningDisplay(false);
	nameGenerator->SetDirectory(dirName);

	typedef std::vector< std::string >    SeriesIdContainer;
	const SeriesIdContainer & seriesUID = nameGenerator->GetSeriesUIDs();
	SeriesIdContainer::const_iterator seriesItr = seriesUID.begin();
	SeriesIdContainer::const_iterator seriesEnd = seriesUID.end();
	
	seriesItr = seriesUID.begin();
	while (seriesItr != seriesUID.end())
	{
		std::string seriesIdentifier;
	
		seriesIdentifier = seriesItr->c_str();
		seriesItr++;
	
		typedef std::vector< std::string >   FileNamesContainer;
		FileNamesContainer fileNames;
		fileNames = nameGenerator->GetFileNames(seriesIdentifier);
	
		ReaderType::Pointer reader = ReaderType::New();
		typedef itk::GDCMImageIO       ImageIOType;
		ImageIOType::Pointer dicomIO = ImageIOType::New();
		reader->SetImageIO(dicomIO);
		reader->SetFileNames(fileNames);
		reader->Update();
		return reader->GetOutput();

		// //Re-orient image
		//typedef itk::OrientImageFilter<ImageType, ImageType> ReOrientorType;
		//auto reOrientor = ReOrientorType::New();
		//reOrientor->UseImageDirectionOn();
		//reOrientor->SetDesiredCoordinateOrientation(itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RAI);
		//reOrientor->SetInput(reader->GetOutput());
		////reOrientor->Update();
		//return reOrientor->GetOutput();

	
		//typedef itk::ImageFileWriter< ImageType > WriterType;
		//WriterType::Pointer writer = WriterType::New();
		//std::string outFileName;
		//outFileName = "E:/test/itkread.nii";
		//writer->SetFileName(outFileName);
		//writer->UseCompressionOn();
		//writer->SetInput(reOrientor->GetOutput());
		//std::cout << "Writing: " << outFileName << std::endl;
		//writer->Update();
	}

}

