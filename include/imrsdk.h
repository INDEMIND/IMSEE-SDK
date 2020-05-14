/*!
 * \brief This header contain the interface of Software Develop Kit
 * \file imrsdk.h
 */
#ifndef _IMRSDK_H_
#define _IMRSDK_H_
#include "imrdata.h"
#include "types.h"

#ifdef WIN32
#ifdef indem_EXPORTS
#define INDEM_DLL_EXPORT __declspec(dllexport)
#else
#define INDEM_DLL_EXPORT __declspec(dllimport)
#endif
#define CALL_METHOD __cdecl
#else
#define CALL_METHOD
#define INDEM_DLL_EXPORT
#endif

/// \~chinese SDK版本
/// \~english SDK version
#define MRSDK_VERSION 6

/// \~chinese 预定义的插件返回码，插件不存在
/// \~english plugin return value: plugin is not exists
#define PLG_NOT_EXIST 1

struct MrDetectionData;
/**
 * \~chinese \brief 数据回调函数
 * \~chinese \param iType
 * 返回的信息码,对ImrModulePose，返回码为SlamPlugin.h中的枚举值SlamStatus
 * \~chinese \param pData 设备数据，例如ImrModulePose,ImrTrig
 * \~chinese \param pParam 用户定义的参数
 */
// typedef void(CALL_METHOD *DataCallback)(int, void *pData, void *pParam);
using DataCallback = std::function<void(int, void *pData, void *pParam)>;
/**
 * \~chinese \brief 设备IMU数据回调函数
 * \~chinese \param pParam 用户定义的参数
 */
// typedef void(CALL_METHOD *ModuleIMUCallback)(indem::ImuData imu);
using ModuleIMUCallback = std::function<void(indem::ImuData imu)>;
/**
 * \~chinese \brief 设备图像数据回调函数
 * \~chinese \param pParam 用户定义的参数
 */
// typedef void(CALL_METHOD *ModuleImageCallback)(double time,
//                                               unsigned char *pLeft,
//                                               unsigned char *pRight, int
//                                               width, int height, int channel,
//                                               void *pParam);
using ModuleImageCallback =
    std::function<void(double time, unsigned char *pLeft, unsigned char *pRight,
                       int width, int height, int channel, void *pParam)>;
/**
 * \~chinese \brief 图像数据回调函数
 */
// typedef void(CALL_METHOD *ImgCallback)(double time, cv::Mat left,
//                                       cv::Mat right);
using ImgCallback =
    std::function<void(double time, cv::Mat left, cv::Mat right)>;
/**
 * \~chinese \brief 矫正图回调函数
 */
// typedef void(CALL_METHOD *RectifiedImgCallback)(double time, cv::Mat left,
//                                                cv::Mat right);
using RectifiedImgCallback =
    std::function<void(double time, cv::Mat left, cv::Mat right)>;
/**
 * \~chinese \brief 视差图回调函数
 */
// typedef void(CALL_METHOD *DisparityCallback)(double time, cv::Mat disparity);
using DisparityCallback = std::function<void(double time, cv::Mat disparity)>;
/**
 * \~chinese \brief 深度图回调函数
 */
// typedef void(CALL_METHOD *DepthCallback)(double time, cv::Mat depth);
using DepthCallback = std::function<void(double time, cv::Mat depth)>;
/**
 * \~chinese \brief 点云回调函数
 */
// typedef void(CALL_METHOD *PointCloudCallback)(double time, cv::Mat points);
using PointCloudCallback = std::function<void(double time, cv::Mat points)>;
/**
 * \~chinese \brief 检测图回调函数
 */
using DetectorImgCallback = std::function<void(indem::DetectorInfo info)>;
/**
 * \~chinese \brief 设备断开连接回调函数
 * \~chinese \param iType 返回的信息码,设备接入为1,断开为0
 * \~chinese \param pParam 用户定义的参数
 */
// typedef void(CALL_METHOD *HotplugCallback)(int iType, void *pParam);
using HotplugCallback = std::function<void(int iType, void *pParam)>;
/**
 * \~chinese \brief 插件回调函数
 * \~chinese \param ret 回调传出的状态码
 * \~chinese \param pResult 插件传出的数据结构指针
 * \~chinese \param pParam 用户在添加回调函数时传入的参数
 */
typedef void(CALL_METHOD *PluginCallback)(int ret, void *pResult, void *pParam);
// using PluginCallback =
//    std::function<void(int ret, void *pResult, void *pParam)>;

namespace indem {
enum IMG_RESOLUTION {
  IMG_640,     // 640X400
  IMG_1280,    // 1280X800
  IMG_DEFAULT, // 640X400
};
/**
 * \~chinese @brief SDK的初始化配置
 * \~english @brief init config
 */
struct MRCONFIG {
  /// \~chinese 设置捕获路径,空表示不捕获数据
  /// \~english capture video to path if use
  char capturePath[128] = {'\0'};
  /// \~chinese 设置数据载入路径
  /// \~english load video from path if use
  char loadPath[128] = {'\0'};
  /// \~chinese 是否使用slam，使用slam=true
  /// \~english use slam if true, other wise false
  bool bSlam = false;
  /// \~chinese 设置用户自定义的slam动态库路径,空表示使用默认slam
  /// \~english path of libslam.so, current directory if not setting
  char slamPath[128] = {'\0'};
  /// \~chinese 设置IMU频率,最大1000Hz
  /// \~english start driver with imuFrequency. Max value is 1000Hz
  int imuFrequency = 1000;
  /// \~chinese 设置IMG频率,最大200Hz
  /// \~english start driver with imgFrequency. Max value is 200Hz
  int imgFrequency = 50;
  /// \~chinese 设置获取位姿结果频率，最大等于IMU频率
  /// \~english get slam pose with poseFrequency. Max value is imuFrequency.
  /// \~english if bSlam is false, nothing will be get.
  int poseFrequency = 0;
  /// \~chinese 图像分辨率，默认640X400
  /// \~english image resolution. Default value is 640*400.
  IMG_RESOLUTION imgResolution = IMG_DEFAULT;
  MRCONFIG(int){};
};

enum MRCapbility {
  GPU_NVidia,
};

/**
 * \~chinese \brief
 * 注册设备热插拔回调函数,在构造函数之后即可调用,不需要在init函数之后调用
 * \~chinese \param cb 回调函数
 * \~chinese \param param 传给回调函数使用的参数
 */
INDEM_DLL_EXPORT void RegistDisconnectCallback(HotplugCallback cb, void *param);

class CIMRSDKImpl;

/**
 * @brief This is SDK interface
 */
class INDEM_DLL_EXPORT CIMRSDK {
public:
  CIMRSDK();
  ~CIMRSDK();
  /**
   * \~chinese \brief 初始化配置，并启动数据接收线程及算法线程
   * \~english \brief start all thread to get driver data and pose with config
   * \~chinese \param config 指定配置
   * \~english \param config if config is not use, all default config will be
   * set
   */
  bool Init(MRCONFIG config = {0});
  /**
   * \~chinese \brief 释放SDK的资源
   * \~english \brief release all resources which SDK used
   */
  void Release();

  /**
   * \~chinese \brief 检测硬件是否支持指定的能力
   * \~chinese \param cap 枚举值,传递需要检测的设备能力
   * \~chinese \return 0-不支持,1-支持，2-硬件支持但与SDK版本不匹配
   * \~english \brief depreciate
   */
  int GetCapbility(MRCapbility cap);

  /**
   * \~chinese \brief 查询模组信息
   * \~english \brief query module informations
   */
  ModuleInfo GetModuleInfo();

  /**
   * \~chinese \brief 查询模组标定信息
   * \~english \brief query calibration of module
   */
  MoudleAllParam GetModuleParams();

  /**
   * \~chinese \brief 添加位姿回调函数
   * \~chinese \param cb 对应算法处理完毕后，用于数据传出的回调函数
   * \~chinese \param param 传给回调函数使用的参数
   * \~english \brief add callback function to get pose data
   * \~english \param cb callback function to regist
   * \~english \param param pass to callback function
   */
  void RegistModulePoseCallback(DataCallback cb, void *param);

  /**
   * \~chinese \brief 注册相机原始图像回调函数
   * \~chinese \param cb 数据传出的回调函数，同步模式
   * \~chinese \param param 传给回调函数使用的参数
   * \~english \brief add callback function to get image data
   * \~english \param cb callback function to regist
   * \~english \param param pass to callback function
   */
  void RegistModuleCameraCallback(ModuleImageCallback cb, void *param);
  /**
   * \~chinese \brief 注册相机图像回调函数
   * \~chinese \param cb 数据传出的回调函数
   * \~english \brief add callback function to get rectified image data
   * \~english \param cb callback function to regist
   */
  void RegistImgCallback(ImgCallback cb);
  /**
   * \~chinese \brief 注册相机矫正图回调函数
   * \~chinese \param cb 数据传出的回调函数
   * \~english \brief add callback function to get rectified image data
   * \~english \param cb callback function to regist
   */
  void RegistRectifiedImgCallback(RectifiedImgCallback cb);

  /**
   * \~chinese \brief 注册相机视差图回调函数
   * \~chinese \param cb 数据传出的回调函数
   * \~english \brief add callback function to get disparity data
   * \~english \param cb callback function to regist
   */
  void RegistDisparityCallback(DisparityCallback cb);

  /**
   * \~chinese \brief 注册相机深度图回调函数
   * \~chinese \param cb 数据传出的回调函数
   * \~english \brief add callback function to get depth data
   * \~english \param cb callback function to regist
   */
  void RegistDepthCallback(DepthCallback cb);

  /**
   * \~chinese \brief 注册相机点云图回调函数
   * \~chinese \param cb 数据传出的回调函数
   * \~english \brief add callback function to get point cloud
   * \~english \param cb callback function to regist
   */
  void RegistPointCloudCallback(PointCloudCallback cb);

  /**
   * \~chinese \brief 注册相机检测图回调函数
   * \~chinese \param cb 数据传出的回调函数
   * \~english \brief add callback function to get detector image
   * \~english \param cb callback function to regist
   */
  void RegistDetectorCallback(DetectorImgCallback cb);

  /**
   * \~chinese \brief 注册头显原始IMU回调函数
   * \~chinese \param cb 数据传出的回调函数，同步模式
   * \~chinese \param param 传给回调函数使用的参数
   * \~english \brief add callback function to get imu data
   * \~english \param cb callback function to regist
   * \~english \param param pass to callback function
   */
  void RegistModuleIMUCallback(ModuleIMUCallback cb);

  /**
   * \~chinese \brief 在加载slam动态库后调用Init之前，进行某些自定义设置
   * \~english \brief data callback function
   * \~chinese @param inParam 需要传的参数
   * \~english \param inParam pass params to slam before it inited
   */
  void BeforeSlamInit(void *inParam);

  /**
   * \~chinese \brief 以指定的值重新初始化slam,如果slam动态库实现了初始化操作
   * \~chinese @param t     时间
   * \~chinese @param pos   位置[x,y,z]
   * \~chinese @param quart 姿态
   * \~chinese @param speed 速度
   * \~chinese @param bias  偏差
   */
  bool ReInitSlam(double t, float *pos, float *quart, float *speed,
                  float *bias);

  /**
   * \~chinese \brief 导入地图
   * \~chinese \param fullpath 导出文件的全路径
   * \~chinese \return 成功返回true
   * \~english \brief import map to slam module
   * \~english \param fullpath fullpath of the map
   * \~english \return true if success, else false.
   */
  bool ImportMap(const char *fullpath);
  /**
   * \~chinese \brief 导出地图
   * \~chinese \param fullpath 导出文件的全路径
   * \~chinese \return 成功返回true
   * \~english \brief export map from slam module
   * \~english \param fullpath fullpath of the map which wante to save
   * \~english \return true if success, else false.
   */
  bool ExportMap(const char *fullpath);

  /**
   * \~chinese @brief 调用Slam插件预定义的接口函数
   * \~chinese @param methodName     指定的函数名称
   * \~chinese @param inParam        需要传给函数的参数
   * \~chinese @param outParam       函数传出的结果
   * \~chinese @return   状态码,成功为0;其他应该定义为该函数的返回值
   */
  int InvokeSlamMethod(const char *methodName, void *inParam, void *outParam);

  /**
   * \~chinese @brief 在插件调用Init方法前做一些参数设置等操作
   * \~chinese @param pluginName     插件的名称
   * \~chinese @param param          传入的相关参数
   * \~english \brief some plugin may be init with params. This interface pass
   * params to Init() function of plugins.
   * \~english \param pluginName which plugin should be given.
   * \~english \param param params to be pass.
   */
  void BeforePluginInit(const char *pluginName, void *param);

  /**
   * \~chinese @brief 手工加载插件,如果已经存在则不做任何操作
   * \~chinese @param pluginName     插件的名称,存放在plugin文件夹下
   * \~chinese @return   状态码,成功为0,已经存在则返回1,失败返回-1
   * \~english \brief load plugin manually.
   * \~english \param pluginName which plugin should be loaded.
   */
  int LoadPlugin(const char *pluginName);

  /**
   * \~chinese @brief
   * 为插件添加一个新的回调函数,如果两个名字相同,新的回调函数将替换掉旧的回调函数,如果插件回调函数数量为0,那么插件功能将关闭
   * \~chinese @param pluginName     插件的名称
   * \~chinese @param callbackName   给回调函数指定的名称
   * \~chinese @param cb             回调函数指针
   * \~chinese @param param          传给回调函数使用的用户数据指针
   * \~chinese @return   状态码,成功为0
   * \~english \brief some plugin may have outputs. Here give a way to get
   * plugin output data.
   * \~english \param pluginName	which plugin should be add.
   * \~english \param callbackName which callback function should be add or
   * replace.
   * \~english \param cb			pointer of callback function.
   * \~english \param param		which will be passed to callback
   * function.
   * \~english \return				true if success
   */
  int AddPluginCallback(const char *pluginName, const char *callbackName,
                        PluginCallback cb, void *param);

  /**
   * \~chinese @brief 调用插件预定义的接口函数
   * \~chinese @param pluginName     插件的名称
   * \~chinese @param methodName     指定的函数名称
   * \~chinese @param inParam        需要传给函数的参数
   * \~chinese @param outParam       函数传出的结果
   * \~chinese @return   状态码,成功为0;其他应该定义为该函数的返回值
   * \~english \brief some plugin may have its own method. Here give a way to
   * invoke these method.
   * \~english \param pluginName	which plugin should be added.
   * \~english \param methodName	which method should be called.
   * \~english \param inParam		pointer passed in.
   * \~english \param outParam		pointer to retrieved.
   * \~english \return				false if fail
   */
  int InvokePluginMethod(const char *pluginName, const char *methodName,
                         void *inParam, void *outParam);

  /**
   * \~chinese @brief 卸载插件,如果不存在则不做任何操作
   * \~chinese @param pluginName     插件的名称,存放在plugin文件夹下
   * \~english \brief release plugin manually.
   * \~english \param pluginName which plugin should be release.
   */
  void ReleasePlugin(const char *pluginName);

  /**
   * \~chinese @brief 使能图像矫正计算
   * \~chinese @return 若成功返回true，否则false
   * \~english \brief enable rectify process.
   * \~english @return if success return true.
   */
  bool EnableRectifyProcessor();

  /**
   * \~chinese @brief 使能视察计算
   * \~chinese @return 若成功返回true，否则false
   * \~english \brief enable disparity process.
   * \~english @return if success return true.
   */
  bool EnableDisparityProcessor();

  /**
   * \~chinese @brief 使能深度计算
   * \~chinese @return 若成功返回true，否则false
   * \~english \brief enable depth process.
   * \~english @return if success return true.
   */
  bool EnableDepthProcessor();

  /**
   * \~chinese @brief 使能点云计算
   * \~chinese @return 若成功返回true，否则false
   * \~english \brief enable point cloud process.
   * \~english @return if success return true.
   */
  bool EnablePointProcessor();
  /**
   * \~chinese @brief 使能深度学习计算
   * \~chinese @return 若成功返回true，否则false
   * \~english \brief enable detector process.
   * \~english @return if success return true.
   */
  bool EnableDetectorProcessor();
  /**
   * \~chinese @brief 取消所有深度相关计算
   * \~chinese @return 若成功返回true，否则false
   * \~english \brief disanable all processors.
   * \~english @return if success return true.
   */
  bool DisableAllProcessors();
  /**
   * \~chinese @brief 使能左右一致性检测
   * \~chinese @return 若成功返回true，否则false
   * \~english \brief enable L-R Consistency check.
   * \~english @return if success return true.
   */
  bool EnableLRConsistencyCheck();
  /**
   * \~chinese @brief 取消使能左右一致性检测
   * \~chinese @return 若成功返回true，否则false
   * \~english \brief disanable L-R Consistency check.
   * \~english @return if success return true.
   */
  bool DisableLRConsistencyCheck();
  /**
   * \~chinese @brief 设置深度计算模式
   * \~chinese @return 若成功返回true，否则false
   * \~english \brief Set depth calculate mode
   * \~english @return if success return true.
   */
  bool SetDepthCalMode(indem::DepthCalMode mode);

  /**
   * \~chinese @brief 显示当前加载的所有插件
   * \~chinese @param pluginNum      插件的数量
   * \~chinese @param pluginName
   * 每个插件的名称,需要传入足够大的空间来存储返回结果
   * \~english @brief display all plugins loaded currently
   * \~english @param pluginNum      plugin's count retrieved
   * \~english @param pluginName     plugin's names retrieved
   */
  static void ListPluginsInfo(int *pluginNum, char **pluginsName);

  /**
   * \~chinese @brief 显示插件的信息
   * \~chinese @param pluginName     要查看的插件的名称
   * \~chinese @param major          插件主版本号
   * \~chinese @param minor          插件次版本号
   * \~chinese @param developer      插件开发团队名称
   * \~english @brief display all plugin's information
   * \~english @param pluginName     which plugin information to be query
   * \~english @param major..........plugin's major version
   * \~english @param minor..........plugin's minor version
   * \~english @param developer......plugin's developer name
   */
  static void ListPluginInfo(const char *pluginsName, int *major, int *minor,
                             char *developer);

private:
  CIMRSDKImpl *m_pImpl;
};

} // namespace indem
#endif