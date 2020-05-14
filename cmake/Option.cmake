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

include(${CMAKE_CURRENT_LIST_DIR}/IncludeGuard.cmake)
cmake_include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/Utils.cmake)


# build components

option(WITH_API "Build with API layer, need OpenCV" ON)

# 3rdparty components

option(WITH_BOOST "Include Boost support" OFF)

# How to install glog?
#   Ubuntu: `sudo apt-get install libgoogle-glog-dev`
option(WITH_GLOG "Include glog support" OFF)

# packages
if(WITH_API)
  include(${CMAKE_CURRENT_LIST_DIR}/DetectOpenCV.cmake)
endif()

#if(WITH_BOOST)
#  find_package(Boost QUIET COMPONENTS filesystem)
#  if(Boost_FOUND)
#    set(Boost_VERSION_STRING "${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION}")
#    set(WITH_FILESYSTEM TRUE)
#    set(WITH_BOOST_FILESYSTEM TRUE)
#  endif()
#endif()
#
#if(NOT WITH_FILESYSTEM)
#  if(MSVC OR MSYS OR MINGW)  # win
#    set(WITH_FILESYSTEM TRUE)
#    set(WITH_NATIVE_FILESYSTEM TRUE)
#  endif()
#endif()

if(WITH_GLOG)
  find_package(glog REQUIRED)
endif()

find_package(CUDA QUIET)

# summary

set_version_values(CMAKE_CXX_COMPILER_VERSION)

status("")
status("Platform:")
status("  HOST_OS: ${HOST_OS}")
status("  HOST_NAME: ${HOST_NAME}")
status("  HOST_ARCH: ${HOST_ARCH}")
status("  HOST_COMPILER: ${CMAKE_CXX_COMPILER_ID}")
status("    COMPILER_VERSION: ${CMAKE_CXX_COMPILER_VERSION}")
status("    COMPILER_VERSION_MAJOR: ${CMAKE_CXX_COMPILER_VERSION_MAJOR}")
status("    COMPILER_VERSION_MINOR: ${CMAKE_CXX_COMPILER_VERSION_MINOR}")
status("    COMPILER_VERSION_PATCH: ${CMAKE_CXX_COMPILER_VERSION_PATCH}")
status("    COMPILER_VERSION_TWEAK: ${CMAKE_CXX_COMPILER_VERSION_TWEAK}")
if(CUDA_FOUND)
  status("  CUDA_VERSION: ${CUDA_VERSION}")
  status("    CUDA_VERSION_MAJOR: ${CUDA_VERSION_MAJOR}")
  status("    CUDA_VERSION_MINOR: ${CUDA_VERSION_MINOR}")
  status("    CUDA_VERSION_STRING: ${CUDA_VERSION_STRING}")
endif()
if(OpenCV_FOUND)
  status("  OpenCV_VERSION: ${OpenCV_VERSION}")
  status("    OpenCV_VERSION_MAJOR: ${OpenCV_VERSION_MAJOR}")
  status("    OpenCV_VERSION_MINOR: ${OpenCV_VERSION_MINOR}")
  status("    OpenCV_VERSION_PATCH: ${OpenCV_VERSION_PATCH}")
  status("    OpenCV_VERSION_TWEAK: ${OpenCV_VERSION_TWEAK}")
  status("    OpenCV_VERSION_STATUS: ${OpenCV_VERSION_STATUS}")
  status("    OpenCV_WITH_WORLD: ${WITH_OPENCV_WORLD}")
endif()
if(indemind_VERSION)
  status("  INDEMIND_VERSION: ${indemind_VERSION}")
  status("    INDEMIND_VERSION_MAJOR: ${indemind_VERSION_MAJOR}")
  status("    INDEMIND_VERSION_MINOR: ${indemind_VERSION_MINOR}")
  status("    INDEMIND_VERSION_PATCH: ${indemind_VERSION_PATCH}")
  status("    INDEMIND_VERSION_TWEAK: ${indemind_VERSION_TWEAK}")
endif()

#if(WITH_BOOST)
#  if(Boost_FOUND)
#    status("    Boost: YES")
#    status("    Boost_VERSION: ${Boost_VERSION_STRING}")
#    #status("    Boost_LIBRARIES: ${Boost_LIBRARIES}")
#  else()
#    status("    Boost: NO")
#  endif()
#endif()

status("  WITH_GLOG: ${WITH_GLOG}")
if(WITH_GLOG)
  if(glog_FOUND)
    status("    glog: YES")
    status("    glog_VERSION: ${glog_VERSION}")
  else()
    status("    glog: NO")
  endif()
endif()

# status("")
# status("Features:")
# status("  Filesystem: "
#  IF WITH_BOOST_FILESYSTEM "boost"
#  ELIF WITH_NATIVE_FILESYSTEM "native"
#  ELSE "none"
# )

status("")
