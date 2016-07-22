/*
Author: QIN Shuo
Date: 2016/6/28
Description:
	Test registration function
	In this version, I use affine transform instead of rigidbody transform
	
	Transform point from with the affine transform we get

	Command
	ANTS.exe 3 -m MI["xxx.nii", ch2.nii, 1, 32] -i 0 -o newww.nii --rigid-affine false
	WarpImageMultiTransform.exe 3 ch2.nii newdeformed.nii newwwAffine.txt -R xxx.nii
*/

#include <string>
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

int itkRegistrationTest()
{
	const unsigned int Dimension = 3;
	typedef float									PixelType;
	typedef itk::Image<PixelType, Dimension>		ImageType;

#pragma region Registration

	// Registration and surface re-construction
	std::string moving_name = "./temp/user_temp.nii";
	std::string fix_name = "";//"./configFile/Atlas/reference_brain_res.nii";
	std::string warp_prefix = "./temp/transform";
	std::string warp_name = "./temp/data_reg.nii";
	std::string affine_txt = "./temp/transformAffine.txt";

	const std::string ANTs_exe = "ANTs.exe";
	const std::string WarpImage_exe = "WarpImageMultiTransform.exe";
	std::string cmd1 = ANTs_exe + " 3 -m MI[\"";
	cmd1.append(fix_name); cmd1.append("\",\""); cmd1.append(moving_name); cmd1.append("\",1,32] -i 0 -o \""); cmd1.append(warp_prefix); cmd1.append("\" --rigid-affine false \n");
	std::string cmd2 = WarpImage_exe + " 3 ";
	cmd2.append(moving_name); cmd2.append(" "); cmd2.append(warp_name); cmd2.append(" "); cmd2.append(affine_txt); cmd2.append(" -R "); cmd2.append(fix_name);

	std::cout << "=======================Runing command1 ====================\n" << cmd1 << std::endl;
	system(cmd1.c_str());
	std::cout << "=======================Command 1 Done =====================\n\n\n\n" << std::endl;

	std::cout << "=======================Runing command2 ====================\n" << cmd2 << std::endl;
	system(cmd2.c_str());
	std::cout << "=======================Command 2 Done =====================\n\n\n\n" << std::endl;


#pragma endregion

	return 0;
}






