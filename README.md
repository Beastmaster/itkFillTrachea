
# Description: #

1. Skin 3d reconstruction from MRI data
2. Trachea gray value equals background thus leaving a hole
3. Use itk tools to fill this hole to achieve a better 3d model

##Toolkits:##
1. ITK 4.8 or later
2. VTK 6.3 or later
3. Advanced normalization toolkit(ANTs)


##Workflow##
1. Call run_command.py to pre-process the data. This script call extract_skin. This command register brain template to user file and enlarge the brain, mask the brain to user file, binarize user file.
2. Call matlab script extract_skin.m. This script fill holes in binary image, and reserve largest connected component.




