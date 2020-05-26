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

cmake_minimum_required(VERSION 2.8.3)
project(imsee_ros_wrapper)

get_filename_component(SDK_DIR "${PROJECT_SOURCE_DIR}/../../.." ABSOLUTE)
include(${SDK_DIR}/cmake/Common.cmake)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# packages

function(checkPackage package customMessage)
  set(varName "${package}_FOUND")
  if (NOT "${${varName}}")
    string(REPLACE "_" "-" aptPackage ${package})
    if("${customMessage}" STREQUAL "")
      message(FATAL_ERROR "\n\n ${package} is missing, please try to install it with:\n sudo apt-get install ros-$(rosversion -d)-${aptPackage}\n\n")
    else()
      message(FATAL_ERROR "\n\n ${customMessage} \n\n")
    endif()
  endif()
endfunction(checkPackage)

find_package(catkin REQUIRED COMPONENTS
  cv_bridge
  geometry_msgs
  image_transport
  message_generation
  nodelet
  roscpp
  sensor_msgs
  std_msgs
  tf
)

generate_messages(
   DEPENDENCIES
   std_msgs
)

checkPackage("cv_bridge" "")
checkPackage("geometry_msgs" "")
checkPackage("image_transport" "")
checkPackage("message_generation" "")
checkPackage("nodelet" "")
checkPackage("roscpp" "")
checkPackage("sensor_msgs" "")
checkPackage("std_msgs" "")
checkPackage("tf" "")

catkin_package(
  CATKIN_DEPENDS cv_bridge geometry_msgs image_transport message_runtime nodelet roscpp sensor_msgs std_msgs tf
)

if(OS_UBUNTU)
  set(MNN_LIB_DIR ${SDK_DIR}/src/detector/lib/x86-64)
  set(INDEMIND_LIB_DIR ${SDK_DIR}/lib/x86-64)
else()
  set(MNN_LIB_DIR ${SDK_DIR}/src/detector/lib/aarch64)
  set(INDEMIND_LIB_DIR ${SDK_DIR}/lib/aarch64)
endif ()

#LIST(APPEND CMAKE_PREFIX_PATH ${SDK_DIR}/_install/lib/cmake)

LIST(APPEND CMAKE_MODULE_PATH cmake)


find_package(OpenCV REQUIRED)

# targets

add_compile_options(-std=c++11)

include_directories(
  ${catkin_INCLUDE_DIRS}
  ${SDK_DIR}/include
)

set(LINK_LIBS
  ${catkin_LIBRARIES}
  ${OpenCV_LIBS}
  ${INDEMIND_LIB_DIR}/libindemind.so
)

add_library(imsee_wrapper src/wrapper_nodelet.cc)
target_link_libraries(imsee_wrapper ${LINK_LIBS})
add_dependencies(imsee_wrapper ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

add_executable(imsee_wrapper_node src/wrapper_node.cc)
target_link_libraries(imsee_wrapper_node imsee_wrapper ${LINK_LIBS})

if(MSVC OR MSYS OR MINGW)
  target_compile_definitions(imsee_wrapper_node
    PUBLIC GLOG_NO_ABBREVIATED_SEVERITIES
  )
endif()

# install

#install(PROGRAMS
#  scripts/get_device_info.py
#  DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
#)

install(TARGETS imsee_wrapper imsee_wrapper_node
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

install(DIRECTORY launch/
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}/launch
)

install(DIRECTORY rviz/
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}/rviz
)

install(FILES nodelet_plugins.xml
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
)