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

如果用户已有opencv可以跳过该步骤。我们会提供基于opencv３.3.1(ROS自带版本)及opencv３.4.3编译的两个版本的库文件

```
[compiler] sudo apt-get install build-essential
[required] sudo apt-get install libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev

$ git clone https://github.com/opencv/opencv.git
$ cd opencv/
$ git checkout tags/3.4.3

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
运行样例
```
sudo ./demo/output/bin/get_image # 运行样例需要用权限
```

#### windows下编译样例

**准备软件**
1. [CMake(3.0以上)](https://github.com/Kitware/CMake/releases/download/v3.17.2/cmake-3.17.2-win64-x64.msi)
2. [Visual Studio 2019](https://download.visualstudio.microsoft.com/download/pr/0fed0c12-ccd3-4767-b151-a616aaf99d86/9aa953f461b054c7428d62c1a3ed2744776621659121b804a1b0bd79bf2b3e5a/vs_Community.exe)
3. [opencv3.3.1](https://sourceforge.net/projects/opencvlibrary/files/opencv-win/3.3.1/opencv-3.3.1-vc14.exe/download)
4. IMSEE-SDK(双目SDK)

**配置准备**
1. 将CMake下bin目录路径添加到系统环境变量"PATH"中。
2. 设置后缀格式为"sln"的项目文件的默认打开方式为"Microsoft Visual Studio 2019"。

**编译步骤**
1. vs2019编译opencv3.3.1
    1. 双击"opencv-3.3.1-vc14.exe"，解压文件到指定目录下。例如解压目录为："D:\\...\opencv331"。
    2. 启动CMake，在"Where is the source code"中输入opencv源码路径。例如："D:\\...\opencv331\opencv\sources";
        在"Where to build the binaries"中输入"opencv构建目录"。例如："D:\\...\opencv331\opencv\build-win10-x64-vs19"。
    3. 点击"Configure"按钮，选择编译工具为"Visual Studio 16 2019"，选择编译目标平台为"x64"。
        点击"Finish"按钮，此时CMake进行第一次编译配置。
    4. 第一次配置"Configuring done"后，在配置窗口中选中"BUILD_opencv_world"，取消选中"BUILD_DOCS"、"BUILD_EXAMPLES"、"BUILD_TESTS"(取消选中可加速opencv编译)。
        再点击"Configure"按钮，进行第二次编译配置。
        第二次配置"Configuring done"后，点击"Generate"按钮，进行win项目生成。
        "Generating done"后，点击"Open Project"按钮(或直接用vs2019打开构建目录下的OpenCV.sln)。
    5. 在vs2019中选择"Release"版本，点击"生成"->"生成解决方案"，开始编译opencv。
    生成的文件在"opencv构建目录\bin\Release"下。
    6. 将"opencv构建目录\bin\Release"添加到环境变量"PATH"。例如："D:\\...\opencv331\opencv\build-win10-x64-vs19\bin\Release"。
    7. 新建系统变量"OpenCV_DIR"，值为"opencv构建目录"。例如："D:\\...\opencv331\opencv\build-win10-x64-vs19"。

2. vs2019编译"IMSEE-SDK\demo"
    1. 双击文件"IMSEE-SDK\demo\build-demo.bat"，会自动打开"cmake-gui.exe"。注意："build-demo.bat"会自动关闭，不要手动关闭。
    2. 点击"Configure"按钮，选择编译工具为"Visual Studio 16 2019"，选择编译目标平台为"x64"。
        点击"Finish"按钮，此时CMake进行编译配置。
	3. "Configuring done"后，点击"Generate"按钮，进行win项目生成。
	4. "Generating done"后，关闭"cmake-gui.exe"。脚本自动用vs2019打开"build/indemind_demos.sln"。
	5. 在vs2019中选择"Release"版本，点击"生成"->"生成解决方案"，开始编译demo。生成的文件在"IMSEE-SDK\demo\output\bin"下。

3. 运行demo
    1. 转到目录"IMSEE-SDK\demo\output\bin"，运行目录下的各个demo。

**注意事项**
1. 编译目标平台要指定为"x64"。
2. 使用"Visual Studio 2015"或其他编译工具时，与上面步骤相同。


#### 样例介绍

##### get_image

例程 get_image 展示了如何通过SDK回调获取原始图片并显示，源代码依赖 libindemind.so 与 get_image.cpp

##### get_imu

例程 get_imu 展示了如何通过SDK回调获取imu数据，源代码依赖 libindemind.so 与 get_imu.cpp

##### get_rectified_img

例程 get_rectified_img 展示了如何通过SDK回调获取矫正后的图片并显示，源代码依赖 libindemind.so 与 get_rectified_img.cpp

##### get_disparity

例程 get_disparity 展示了如何通过SDK回调获取视差图并显示，源代码依赖 libindemind.so 与 get_disparity.cpp

##### get_disparity_with_lr_check


例程 get_disparity_with_lr_check 展示了如何通过SDK使能左右一致性检测并通过回调获取视差图显示，源代码依赖 libindemind.so 与 get_image.cpp　客户可以根据源码中相应api设置或取消左右一致性检测, 其中默认模式为取消左右一致性检测；值得注意的是，目前左右一致性检测只在x64 ubuntu起效

##### get_disparity_with_high_accuracy

例程 get_disparity_with_high_accuracy 展示了如何通过SDK使能高精度模式并通过回调获取视差图显示，源代码依赖 libindemind.so 与 get_disparity_with_high_accuracy.cpp 客户可以根据源码中相应api设置高精度或高速深度计算模式, 其中默认模式为高速深度计算模式；值得注意的是，目前高精度模式只在x64 ubuntu起效

##### get_depth


例程 get_depth 展示了如何通过SDK回调获取深度图并显示，源代码依赖 libindemind.so 与 get_depth.cpp

##### get_depth_with_region

例程 get_depth_with_region 展示了如何通过SDK回调获取深度图并显示，同时可显示光标所指像素及周围九个像素的深度值，源代码依赖 libindemind.so 与 get_depth_with_region.cpp

##### get_detector

例程 get_detector 展示了如何通过SDK回调获取深度学习物体检测结果，源代码依赖 libindemind.so 与 get_detector.cpp

##### record_imu

例程 record_imu 展示了如何通过SDK回调获取imu并将数据储存在imudata.txt文件下，源代码依赖 libindemind.so 与 record_imu.cpp

##### get_points

例程 get_points 展示了如何通过SDK回调获取点云图并显示，源代码依赖 libindemind.so，PCL库 与 get_points.cpp　get_points.cpp util_pcl.cpp；如果检测到电脑中不存在PCL库，该例程不会被编译

#### 编译ROS样例

ROS环境准备(以１6.04 ROS Kinetic为例)
```
$ sudo sh -c 'echo "deb http://packages.ros.org/ros/ubuntu $(lsb_release -sc) main" > /
, → etc/apt/sources.list.d/ros-latest.list'
$ sudo apt-key adv --keyserver 'hkp://keyserver.ubuntu.com:80' --recv-key␣
, → C1CF6E31E6BADE8868B172B4F42ED6FBAB17C654
$ sudo apt-get update
$ sudo apt-get install ros-kinetic-desktop-full
$ sudo rosdep init
$ rosdep update
$ echo "source /opt/ros/kinetic/setup.bash" >> ~/.bashrc
$ source ~/.bashrc

```
编译
```
$ cd <sdk>
$ make ros
```
运行
```
$ cd <sdk>
$ sudo su # 进入权限模式
$ source ros/devel/setup.bash
$ roslaunch imsee_ros_wrapper start.launch # 如果您想预览rviz，则运行roslaunch imsee_ros_wrapper display.launch
```
查看发布节点
```
$ rostopic list
```
结果
```
/imsee/camera_info
/imsee/depth
/imsee/depth/compressed
/imsee/depth/compressed/parameter_descriptions
/imsee/depth/compressed/parameter_updates
/imsee/depth/compressedDepth
/imsee/depth/compressedDepth/parameter_descriptions
/imsee/depth/compressedDepth/parameter_updates
/imsee/depth/theora
/imsee/depth/theora/parameter_descriptions
/imsee/depth/theora/parameter_updates
/imsee/disparity
/imsee/disparity/compressed
/imsee/disparity/compressed/parameter_descriptions
/imsee/disparity/compressed/parameter_updates
/imsee/disparity/compressedDepth
/imsee/disparity/compressedDepth/parameter_descriptions
/imsee/disparity/compressedDepth/parameter_updates
/imsee/disparity/theora
/imsee/disparity/theora/parameter_descriptions
/imsee/disparity/theora/parameter_updates
/imsee/image/camera_info
/imsee/image/detector
/imsee/image/detector/compressed
/imsee/image/detector/compressed/parameter_descriptions
/imsee/image/detector/compressed/parameter_updates
/imsee/image/detector/compressedDepth
/imsee/image/detector/compressedDepth/parameter_descriptions
/imsee/image/detector/compressedDepth/parameter_updates
/imsee/image/detector/theora
/imsee/image/detector/theora/parameter_descriptions
/imsee/image/detector/theora/parameter_updates
/imsee/image/left
/imsee/image/left/compressed
/imsee/image/left/compressed/parameter_descriptions
/imsee/image/left/compressed/parameter_updates
/imsee/image/left/compressedDepth
/imsee/image/left/compressedDepth/parameter_descriptions
/imsee/image/left/compressedDepth/parameter_updates
/imsee/image/left/theora
/imsee/image/left/theora/parameter_descriptions
/imsee/image/left/theora/parameter_updates
/imsee/image/rectified/camera_info
/imsee/image/rectified/left
/imsee/image/rectified/left/compressed
/imsee/image/rectified/left/compressed/parameter_descriptions
/imsee/image/rectified/left/compressed/parameter_updates
/imsee/image/rectified/left/compressedDepth
/imsee/image/rectified/left/compressedDepth/parameter_descriptions
/imsee/image/rectified/left/compressedDepth/parameter_updates
/imsee/image/rectified/left/theora
/imsee/image/rectified/left/theora/parameter_descriptions
/imsee/image/rectified/left/theora/parameter_updates
/imsee/image/rectified/right
/imsee/image/rectified/right/compressed
/imsee/image/rectified/right/compressed/parameter_descriptions
/imsee/image/rectified/right/compressed/parameter_updates
/imsee/image/rectified/right/compressedDepth
/imsee/image/rectified/right/compressedDepth/parameter_descriptions
/imsee/image/rectified/right/compressedDepth/parameter_updates
/imsee/image/rectified/right/theora
/imsee/image/rectified/right/theora/parameter_descriptions
/imsee/image/rectified/right/theora/parameter_updates
/imsee/image/right
/imsee/image/right/compressed
/imsee/image/right/compressed/parameter_descriptions
/imsee/image/right/compressed/parameter_updates
/imsee/image/right/compressedDepth
/imsee/image/right/compressedDepth/parameter_descriptions
/imsee/image/right/compressedDepth/parameter_updates
/imsee/image/right/theora
/imsee/image/right/theora/parameter_descriptions
/imsee/image/right/theora/parameter_updates
/imsee/imu
/imsee/points
/rosout
/rosout_agg
/tf_static
```
特别说明：除左右目原始图像及imu以外的节点，只有主动订阅才会发布，这是为了节省运算量

