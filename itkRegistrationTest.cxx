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


/*
Input: 
	moving      : Moving image file name, .nii format
	fix         : Fixed image file name, .nii format
	warp		: Warped moving image, apply registration transform to moving image
	preffix     : Affine transform file preffix, no surfix. The affinetransform file name is: preffix+Affine.txt
	brain       : Oringal brain image, .nii format, this image will be threshold to binary image
	warped_brain: Warped moving brain, apply registration transform to moving brain.
Note:
	All data passed by file
*/
int itkRegistrationTest(std::string moving, std::string fix, std::string warp, std::string preffix, std::string brain,std::string warped_brain)
{
	const unsigned int Dimension = 3;
	typedef float									PixelType;
	typedef itk::Image<PixelType, Dimension>		ImageType;

#pragma region Registration

	// Registration and surface re-construction
	std::string moving_name = moving;// "./temp/user_temp.nii";
	std::string fix_name = fix;//"./configFile/Atlas/reference_brain_res.nii";
	std::string warp_prefix = preffix;// "./temp/transform";
	std::string warp_name = warp;//"./temp/data_reg.nii";
	std::string affine_txt = preffix.append("Affine.txt");//"./temp/transformAffine.txt";
	std::string moving_brain = brain;
	std::string warp_brain = warped_brain;

#ifdef _WIN32
	const std::string ANTs_exe = "ANTs.exe";
	const std::string WarpImage_exe = "WarpImageMultiTransform.exe";
#else
	const std::string ANTs_exe = "./ANTs";
	const std::string WarpImage_exe = "./WarpImageMultiTransform";
#endif

	std::string cmd1 = ANTs_exe + " 3 -m MI[\"";
	cmd1.append(fix_name); cmd1.append("\",\""); cmd1.append(moving_name); cmd1.append("\",1,32] -i 0 -o \""); cmd1.append(warp_prefix); cmd1.append("\" --rigid-affine false \n");
	std::string cmd2 = WarpImage_exe + " 3 ";
	cmd2.append(moving_name); cmd2.append(" "); cmd2.append(warp_name); cmd2.append(" "); cmd2.append(affine_txt); cmd2.append(" -R "); cmd2.append(fix_name);
	
	std::string cmd3 = WarpImage_exe + " 3 ";
	cmd3.append(moving_brain); cmd3.append(" "); cmd3.append(warp_brain); cmd3.append(" "); cmd3.append(affine_txt); cmd3.append(" -R "); cmd3.append(fix_name);cmd3.append(" --use-NN");

	std::cout << "=======================Runing command1 ====================\n" << cmd1 << std::endl;
	system(cmd1.c_str());
	std::cout << "=======================Command 1 Done =====================\n\n\n\n" << std::endl;

	std::cout << "=======================Runing command2 ====================\n" << cmd2 << std::endl;
	system(cmd2.c_str());
	std::cout << "=======================Command 2 Done =====================\n\n\n\n" << std::endl;

	if (!brain.empty())
	{
		std::cout << "=======================Runing command3 ====================\n" << cmd3 << std::endl;
		system(cmd3.c_str());
		std::cout << "=======================Command 3 Done =====================\n\n\n\n" << std::endl;
	}
	
#pragma endregion

	return 0;
}






