# Copyright 2020 Indemind Co., Ltd. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required(VERSION 3.0)

project(indemind_demos VERSION 2.0.0 LANGUAGES C CXX)

get_filename_component(PRO_DIR ${PROJECT_SOURCE_DIR} DIRECTORY)

include(${PRO_DIR}/cmake/Common.cmake)
#include(${PRO_DIR}/cmake/Utils.cmake)

# options

#include(${PRO_DIR}/cmake/Option.cmake)

# flags

if(OS_WIN)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
else()
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

include(${PRO_DIR}/cmake/DetectCXX11.cmake)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")

string(STRIP "${CMAKE_C_FLAGS}" CMAKE_C_FLAGS)
string(STRIP "${CMAKE_CXX_FLAGS}" CMAKE_CXX_FLAGS)
message(STATUS "C_FLAGS: ${CMAKE_C_FLAGS}")
message(STATUS "CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

if(OS_UBUNTU)
  #set(MNN_LIB_DIR ${PRO_DIR}/src/detector/lib/x86-64)
  set(INDEMIND_LIB_DIR ${PRO_DIR}/lib/x86-64)
elseif(OS_WIN)
  #set(MNN_LIB_DIR ${PRO_DIR}/src/detector/lib/aarch64)
  set(INDEMIND_LIB_DIR ${PRO_DIR}/lib/win10-x64)
else ()
  #set(MNN_LIB_DIR ${PRO_DIR}/src/detector/lib/win10-x64)
  set(INDEMIND_LIB_DIR ${PRO_DIR}/lib/aarch64)
endif ()

# packages

LIST(APPEND CMAKE_PREFIX_PATH ${PRO_DIR}/install/lib/cmake)
# find_package(indemind REQUIRED)
# message(STATUS "Found indemind: ${indemind_VERSION}")
include(${PRO_DIR}/cmake/DetectOpenCV.cmake)

# targets

set(OUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/output")


include_directories(
  ${PRO_DIR}/include
)

#include_directories(
#  ${PRO_DIR}/src
#)

set_outdir(
  "${OUT_DIR}/lib"
  "${OUT_DIR}/lib"
  "${OUT_DIR}/bin"
)

# targets

include_directories(
  ${OpenCV_INCLUDE_DIRS}
)


# make_executable2(NAME
#                 [SRCS src1 src2 ...]
#                 [WITH_OPENCV]
#                 [WITH_PCL])
macro(make_executable2 NAME)
set(options WITH_OPENCV WITH_PCL)
set(oneValueArgs)
set(multiValueArgs SRCS)
cmake_parse_arguments(THIS "${options}" "${oneValueArgs}"
                        "${multiValueArgs}" ${ARGN})

IF(WIN32)
set(__link_libs ${INDEMIND_LIB_DIR}/indemind.lib)
ELSE(WIN32)
set(__link_libs ${INDEMIND_LIB_DIR}/libindemind.so)
ENDIF(WIN32)

IF(WIN32)
set(__dll_search_paths ${PRO_DIR}/src/detector/lib/win10-x64;${PRO_DIR}/lib/win10-x64)
ELSE(WIN32)
set(__dll_search_paths ${PRO_DIR}/install/bin)
ENDIF(WIN32)

list(APPEND __link_libs ${OpenCV_LIBS})
list(APPEND __dll_search_paths ${OpenCV_LIB_SEARCH_PATH})

if(THIS_WITH_PCL)
  list(APPEND __link_libs ${PCL_LIBRARIES})
  #list(APPEND __link_libs pcl::pcl)
  list(APPEND __dll_search_paths ${PCL_LIB_SEARCH_PATH})
endif()

add_executable(${NAME} ${THIS_SRCS})
IF(WIN32)
target_link_libraries(${NAME}
# ${MNN_LIB_DIR}/MNN.lib
${__link_libs}
)
ELSE(WIN32)
target_link_libraries(${NAME}
# ${MNN_LIB_DIR}/libMNN.so
${__link_libs}
pthread
)
ENDIF(WIN32)
target_create_scripts(${NAME} DLL_SEARCH_PATHS ${__dll_search_paths})

if(THIS_WITH_PCL)
  target_include_directories(${NAME} PRIVATE ${PCL_INCLUDE_DIRS})
  #target_compile_definitions(${NAME} PRIVATE ${PCL_DEFINITIONS})
  #target_compile_options(${NAME} PRIVATE ${PCL_COMPILE_OPTIONS})
endif()
if(OS_WIN)
  target_compile_definitions(${NAME}
    PUBLIC GLOG_NO_ABBREVIATED_SEVERITIES
  )
endif()
endmacro()

find_package(PCL)
if(PCL_FOUND)
  message(STATUS "Found PCL: ${PCL_VERSION}")

  # message(STATUS "PCL_LIBRARIES: ${PCL_LIBRARIES}")
  message(STATUS "PCL_INCLUDE_DIRS: ${PCL_INCLUDE_DIRS}")
  message(STATUS "PCL_LIBRARY_DIRS: ${PCL_LIBRARY_DIRS}")
  message(STATUS "PCL_DEFINITIONS: ${PCL_DEFINITIONS}")
  message(STATUS "PCL_COMPILE_OPTIONS: ${PCL_COMPILE_OPTIONS}")

  if(OS_WIN)
    get_filename_component(PCL_LIB_SEARCH_PATH "${PCL_LIBRARY_DIRS}/../bin" ABSOLUTE)
  else()
    set(PCL_LIB_SEARCH_PATH "${PCL_LIBRARY_DIRS}")
  endif()
else()
  message(WARNING "PCL not found :(")
endif()

make_executable2(get_depth_with_region SRCS get_depth_with_region.cpp)
make_executable2(get_depth SRCS get_depth.cpp)
make_executable2(get_detector SRCS get_detector.cpp)
make_executable2(get_device_info SRCS get_device_info.cpp)
make_executable2(get_disparity_with_lr_check SRCS get_disparity_with_lr_check.cpp)
make_executable2(get_disparity_with_high_accuracy SRCS get_disparity_with_high_accuracy.cpp)
make_executable2(get_disparity SRCS get_disparity.cpp)
make_executable2(get_image SRCS get_image.cpp)
make_executable2(get_imu SRCS get_imu.cpp)
make_executable2(get_rectified_img SRCS get_rectified_img.cpp)
make_executable2(record_imu SRCS record_imu.cpp)

if(PCL_FOUND)

make_executable2(get_points
SRCS get_points.cpp util_pcl.cpp
WITH_OPENCV WITH_PCL
)

endif()

# execute_process(COMMAND sh ${PRO_DIR}/scripts/cp_files.sh)

