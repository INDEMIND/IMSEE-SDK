#pragma once
//该文件存放SDK对外的数据接口
//坐标系定义：x轴右，y轴上,z轴后
struct ImrPose {
  double time;
  //[x,y,z]
  float _position[3];
  //[w,x,y,z]
  float _rotation[4];
  //[pitch roll psi]
  float _oula[3];
};

//模组位姿
struct ImrModulePose {
  ImrPose _pose;
  double _cov[36]; //权重
};

//模组摄像头图像数据
struct ImrImage {
  double _time;
  int _width;
  int _height;
  unsigned char *_image;
};

struct MrPoint {
  float _x;
  float _y;
  float _z;
  float _flag;
};

//相机标定参数
struct CameraCalibrationParameter {
  int _width;   //图像宽
  int _height;  //图像高
  int _channel; //通道数

  double _Kl[9];             // 3X3 左相机内参矩阵
  double _Kr[9];             // 3X3 右相机内参矩阵
  double _Dl[4];             // 4X1 左相机畸变差校正参数,鱼眼畸变
  double _Dr[4];             // 4X1 右相机畸变差校正参数,鱼眼畸变
  double _Pl[12];            // 3X4 基线校正后左相机投影矩阵
  double _Pr[12];            // 3X4 基线校正后右相机投影矩阵
  double _Rl[9];             // 3X3 基线校正后左相机旋转矩阵
  double _Rr[9];             // 3X3 基线校正后左相机旋转矩阵
  double _TSCl[16];          // 4X4 左相机系到传感器坐标系的变换
  double _TSCr[16];          // 4X4 右相机系到传感器坐标系的变换
  double _focal_length_l[2]; //相机fx,fy
  double _focal_length_r[2]; //相机fx,fy
  double _principal_point_l[2]; //相机cx,cy
  double _principal_point_r[2]; //相机cx,cy
  double _baseline;             //基线，单位：m

  double _AMax;
  double _GMax;
  double _SigmaGC;
  double _SigmaAC;
  double _SigmaBg;
  double _SigmaBa;
  double _SigmaGwC;
  double _SigmaAwC;

  /*  加计参数,3X4矩阵,每个元素如下
   *    x   y   z
   *    11  12  13
   *    21  22  23
   *    31  32  33
   */
  double _Acc[12];
  /*  陀螺参数,3X4矩阵,每个元素如下
   *    x   y   z
   *    11  12  13
   *    21  22  23
   *    31  32  33
   */
  double _Gyr[12];
};