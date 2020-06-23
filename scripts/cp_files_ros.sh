#!/usr/bin/env bash
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

BASE_DIR=$(cd "$(dirname "$0")" && pwd)
PROJ_DIR=$BASE_DIR/..
WRAPPER_OUTPUT_DIR=$PROJ_DIR/ros/devel/lib/imsee_ros_wrapper
DRIVER_BASE_DIR=$PROJ_DIR/src/driver/lib
DETECTOR_BASE_DIR=$PROJ_DIR/src/detector
MNN_CONFIG_DIR=$DETECTOR_BASE_DIR/config
MNN_LIB_BASE_DIR=$DETECTOR_BASE_DIR/lib

# check host arch
HOST_ARCH=$(uname -m)
# echo "Host arch is $HOST_ARCH"

# if [ "$HOST_ARCH" = "x86_64" ]; then
#   DRIVER_DIR=$DRIVER_BASE_DIR/x86-64
#   MNN_LIB_DIR=$MNN_LIB_BASE_DIR/x86-64
# elif [ "$HOST_ARCH" = "aarch64" ]; then
#   DRIVER_DIR=$DRIVER_BASE_DIR/aarch64
#   MNN_LIB_DIR=$MNN_LIB_BASE_DIR/aarch64
# else
#   echo "Unknown host arch :("
#   exit 1
# fi

# creat link
# if [ ! -f "$WRAPPER_OUTPUT_DIR/libusbdriver.so" ]; then
#   ln -s $DRIVER_DIR/libusbdriver.so $WRAPPER_OUTPUT_DIR
#   echo "Finish cp libusbdriver.so!"
# fi

if [ ! -f "$WRAPPER_OUTPUT_DIR/libMNN.so" ]; then
  ln -s $MNN_LIB_DIR/libMNN.so $WRAPPER_OUTPUT_DIR
  echo "Finish cp libMNN.so!"
fi



if [ ! -f "$WRAPPER_OUTPUT_DIR/blazeface.mnn" ]; then
  ln -s $MNN_CONFIG_DIR/blazeface.mnn $WRAPPER_OUTPUT_DIR
  echo "Finish cp blazeface.mnn!"
fi
