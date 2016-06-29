cmake_minimum_required(VERSION 2.8)

set(PRJ connectArea)

project(${PRJ})
 
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
  
 set(
 SRCS
 itkConnectArea.cxx
 itk2vtkReadDicom.cxx
 itkRegistrationTest.cxx
 )
 
 set(
 HEAD
 Register.h
 Register.hpp
 )
 
add_executable(${PRJ} MACOSX_BUNDLE ${SRCS} ${HEAD})

target_link_libraries(${PRJ} ITKReview  ${ITK_LIBRARIES})
target_link_libraries(${PRJ} ${VTK_LIBRARIES})


