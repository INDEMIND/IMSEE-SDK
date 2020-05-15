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
include CommonDefs.mk

MKFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
MKFILE_DIR := $(patsubst %/,%,$(dir $(MKFILE_PATH)))

# CMAKE_INSTALL_PREFIX:
#   https://cmake.org/cmake/help/latest/variable/CMAKE_INSTALL_PREFIX.html
#
#   UNIX: /usr/local
#   Windows: c:/Program Files/${PROJECT_NAME}

# Options
#
#   SUDO: sudo command
#
# e.g. make [TARGET] SUDO=

SUDO ?= sudo
CMAKE_BUILD_EXTRA_OPTIONS ?=

.DEFAULT_GOAL := all

help:
	@echo "Usage:"
	@echo "  make help            show help message"
	@echo "  make init            init project"
	@echo "  make demo            build demo"
	@echo "  make clean           clean generated or useless things"

.PHONY: help

all: init demo ros

.PHONY: all

# init

init:
	@$(call echo,Make $@)
	@$(SH) ./scripts/init.sh $(INIT_OPTIONS)

.PHONY: init

# demo

demo:
	@$(call echo,Make $@)
	@$(call cmake_build,./demo/build)
	sh ./scripts/cp_files.sh
.PHONY: demo

# ros

ros:
	@$(call echo,Make $@)
ifeq ($(HOST_OS),Linux)
	@cd ./ros && catkin_make -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)
	sh ./scripts/cp_files_ros.sh
else
	$(error "Can't make ros on $(HOST_OS)")
endif

.PHONY: ros

cleanros:
	@$(call echo,Make $@)
	@$(call rm,./ros/build/)
	@$(call rm,./ros/devel/)
	@$(call rm,./ros/install/)
	@$(call rm,./ros/.catkin_workspace)
	@$(call rm,./ros/src/CMakeLists.txt)
	@$(call rm_f,*INFO*,$(HOME)/.ros/)
	@$(call rm_f,*WARNING*,$(HOME)/.ros/)
	@$(call rm_f,*ERROR*,$(HOME)/.ros/)
	@$(call rm_f,*FATAL*,$(HOME)/.ros/)

.PHONY: cleanros

# clean

clean:
	@$(call echo,Make $@)
	@$(call rm,./demo/build/)
	@$(call rm,./demo/output/)

