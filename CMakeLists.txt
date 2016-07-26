cmake_minimum_required(VERSION 2.8)

set(PRJ connectArea)

project(${PRJ})
 
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
  
 set(
 SRCS
 main.cpp
 itkConnectArea.cxx
 itk2vtkReadDicom.cxx
 itkRegistrationTest.cxx
 itkTransformTest.cxx
 itkvtkCoordinate.cxx
 vtkMarchingConnected.cpp
 colorSurface.cxx
 search_corresponding_coordinate.cxx
 )
 
 set(
 HEAD
 Register.h
 Register.hpp
 )
 
add_executable(${PRJ} MACOSX_BUNDLE ${SRCS} ${HEAD})

target_link_libraries(${PRJ} ITKReview  ${ITK_LIBRARIES})
target_link_libraries(${PRJ} ${VTK_LIBRARIES})


