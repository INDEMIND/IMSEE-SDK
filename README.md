# IMSEE-SDK

## 概述

IMSEE-SDK是indemind双目惯性模组的软件开发工具包。模组采用“双目摄像头+IMU”多传感器融合架构与微秒级时间同步机制，为视觉SLAM研究提供精准稳定数据源；内置自研高精度Vi-SLAM算法，以满足SLAM研究、智能机器人、无人机避障、室内外导航定位等使用需求。模组运用摄像头+IMU多传感器融合架构，使摄像头与IMU传感器优势互补，实现位姿精度更高、环境适应性更强、动态性能更稳定、成本更低的双目立体视觉硬件方案 。

## SDK

### SDK说明

#### 支持平台

SDK基于cmake构建，跨Ubuntu、Ｗin10平台。其中支持x64/tx2/firefly rk系列等不同架构下Ubuntu的支持。目前已完全测试通过的平台有：

- Ubuntu 16.04 on x64
- Ubuntu 16.04 on TX2
- Ubuntu 16.04 on RK3328

对于Win10等其他平台的支持将会很快实现。

### SDK工程目录介绍

### SDK编译

#### Ubuntu下编译样例

代码获取

```
sudo apt-get install git
git clone https://github.com/indemind/IMSEE-SDK.git
```

准备依赖

```
cd <IMSEE-SDK>  # <IMSEE-SDK> 为SDK具体路径
make init
```
opencv安装

```
[compiler] sudo apt-get install build-essential
[required] sudo apt-get install libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev

$ git clone https://github.com/opencv/opencv.git
$ cd opencv/
$ git checkout tags/3.4.1

$ mkdir build
$ cd build/

$ cmake \
-DCMAKE_BUILD_TYPE=RELEASE \
-DCMAKE_INSTALL_PREFIX=/usr/local \
\
-DWITH_CUDA=OFF \
\
-DBUILD_DOCS=OFF \
-DBUILD_EXAMPLES=OFF \
-DBUILD_TESTS=OFF \
-DBUILD_PERF_TESTS=OFF \
..

$ make -j4
$ sudo make install
```

编译样例
```
cd <IMSEE-SDK>  # <IMSEE-SDK> 为SDK具体路径
make demo
```

