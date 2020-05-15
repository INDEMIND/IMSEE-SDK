# IMSEE-SDK

## 概述

IMSEE-SDK是indemind双目惯性模组的软件开发工具包。模组采用“双目摄像头+IMU”多传感器融合架构与微秒级时间同步机制，为视觉SLAM研究提供精准稳定数据源。模组运用摄像头+IMU多传感器融合架构，使摄像头与IMU传感器优势互补，实现位姿精度更高、环境适应性更强、动态性能更稳定、成本更低的双目立体视觉硬件方案 。

## SDK

### SDK说明

#### 支持平台

SDK基于cmake构建，跨Ubuntu、Ｗin10平台。其中支持x64/tx2/firefly rk系列等不同架构下Ubuntu的支持。目前已完全测试通过的平台有：

- Ubuntu 16.04 on x64
- Ubuntu 16.04 on TX2
- Ubuntu 16.04 on RK3328

对于Win10等其他平台的支持将会很快实现。

### SDK工程目录介绍

#### cmake

cmake文件夹中是一些辅助cmake编译输出文件夹。

#### demo

demo文件夹是SDK的样例集合，里面包含了十余个工程样例代码以及CamkeLists文件，客户可以参考该文件夹移植代码或直接编写实例

#### include

include文件夹包含了SDK所输出的所有头文件，用户在移植工程或依赖libindemind.so时需要包含改文件夹

#### lib

lib文件夹中存放了各个平台编译输出的SDK库文件libindemind.so

#### scripts

scripts文件夹中为辅助编译的脚本

#### src

src文件夹中存放的是SDK中部分可公开给客户的子模块头文件以及动态库，这些动态库是demo样例所需要的

#### third_party
third_party文件夹为libindemind.so所依赖的第三方库，目前包含了Eigen3

#### 其他

另外工程文件中还包含了辅助编译的CommonDefs.mk，Makefile, README和.gitignore文件

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
### 样例介绍

#### get_image

例程 get_image 展示了如何通过SDK回调获取原始图片并显示，源代码依赖 libindemind.so 与 get_image.cpp

#### get_imu

例程 get_imu 展示了如何通过SDK回调获取imu数据，源代码依赖 libindemind.so 与 get_imu.cpp

#### get_rectified_img

例程 get_rectified_img 展示了如何通过SDK回调获取矫正后的图片并显示，源代码依赖 libindemind.so 与 get_rectified_img.cpp

#### get_disparity

例程 get_disparity 展示了如何通过SDK回调获取视差图并显示，源代码依赖 libindemind.so 与 get_disparity.cpp

#### get_disparity_with_lr_check


例程 get_disparity_with_lr_check 展示了如何通过SDK使能左右一致性检测并通过回调获取视差图显示，源代码依赖 libindemind.so 与 get_image.cpp　客户可以根据源码中相应api设置或取消左右一致性检测, 其中默认模式为取消左右一致性检测；值得注意的是，目前左右一致性检测只在x64 ubuntu起效

#### get_disparity_with_high_accuracy

例程 get_disparity_with_high_accuracy 展示了如何通过SDK使能高精度模式并通过回调获取视差图显示，源代码依赖 libindemind.so 与 get_disparity_with_high_accuracy.cpp 客户可以根据源码中相应api设置高精度或高速深度计算模式, 其中默认模式为高速深度计算模式；值得注意的是，目前高精度模式只在x64 ubuntu起效

#### get_depth


例程 get_depth 展示了如何通过SDK回调获取深度图并显示，源代码依赖 libindemind.so 与 get_depth.cpp

#### get_depth_with_region

例程 get_depth_with_region 展示了如何通过SDK回调获取深度图并显示，同时可显示光标所指像素及周围九个像素的深度值，源代码依赖 libindemind.so 与 get_depth_with_region.cpp

#### get_detector

例程 get_detector 展示了如何通过SDK回调获取深度学习物体检测结果，源代码依赖 libindemind.so 与 get_detector.cpp

#### record_imu

例程 record_imu 展示了如何通过SDK回调获取imu并将数据储存在imudata.txt文件下，源代码依赖 libindemind.so 与 record_imu.cpp

#### get_points

例程 get_points 展示了如何通过SDK回调获取点云图并显示，源代码依赖 libindemind.so，PCL库 与 get_points.cpp　get_points.cpp util_pcl.cpp；如果检测到电脑中不存在PCL库，该例程不会被编译

