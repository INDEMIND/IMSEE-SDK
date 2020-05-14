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


# clean

clean:
	@$(call echo,Make $@)
	@$(call rm,./demo/build/)
	@$(call rm,./demo/output/)

