#include "imrdata.h"
#include "imrsdk.h"
#include "logging.h"
#include "times.h"
#include "types.h"
#include <cv_bridge/cv_bridge.h>
#include <dlfcn.h>
#include <fstream>
#include <image_transport/image_transport.h>
#include <iomanip>
#include <nodelet/nodelet.h>
#include <ros/ros.h>
#include <sensor_msgs/Imu.h>
#include <sensor_msgs/PointCloud2.h>
#include <sensor_msgs/Temperature.h>
#include <sensor_msgs/distortion_models.h>
#include <sensor_msgs/image_encodings.h>
#include <sensor_msgs/point_cloud2_iterator.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <tf/tf.h>
#include <tf2_ros/static_transform_broadcaster.h>
#include <visualization_msgs/Marker.h>
#define _ADDRESSREF(v) (&(v))

using namespace indem;
namespace enc = sensor_msgs::image_encodings;
namespace imsee {
class ROSWrapperNodelet : public nodelet::Nodelet {
public:
  ROSWrapperNodelet() {}

  ~ROSWrapperNodelet() {
    if (m_pSDK) {
      m_pSDK->Release();
      delete m_pSDK;
      m_pSDK = NULL;
    }
  }

  ros::Time hardTimeToSoftTime(double hard_time) {
    static bool isInited = false;
    static double soft_time_begin(0);
    static double hard_time_begin(0);

    if (false == isInited) {
      soft_time_begin = ros::Time::now().toSec();
      hard_time_begin = hard_time;
      isInited = true;
    }
    double time_sec_double = hard_time - hard_time_begin;

    return ros::Time(soft_time_begin + time_sec_double);
  }

  void LoadCameraInfo(CameraParameter param) {
    sensor_msgs::CameraInfo *camera_info = new sensor_msgs::CameraInfo();
    camera_info->width = param._width;
    camera_info->height = param._height;
    camera_info->distortion_model = "fisheye";
    for (size_t i = 0; i < 4; i++) {
      camera_info->D.push_back(param._D[i]);
    }
    for (size_t i = 0; i < 9; i++) {
      camera_info->K.at(i) = param._K[i];
    }
    for (size_t i = 0; i < 9; i++) {
      camera_info->R.at(i) = param._R[i];
    }
    for (size_t i = 0; i < 12; i++) {
      camera_info->P.at(i) = param._P[i];
    }
    camera_info_prt = sensor_msgs::CameraInfoPtr(camera_info);
  }

  void publishImages(double time, cv::Mat left, cv::Mat right) {
    std_msgs::Header header_l, header_r;
    header_l.stamp = hardTimeToSoftTime(time);
    header_l.frame_id = "camera_left_frame";
    auto &&msg_l = cv_bridge::CvImage(header_l, enc::MONO8, left).toImageMsg();
    camera_info_prt->header.stamp = msg_l->header.stamp;
    m_left_image_publisher.publish(msg_l, camera_info_prt);

    header_r.stamp = header_l.stamp;
    header_r.frame_id = "camera_right_frame";
    auto &&msg_r = cv_bridge::CvImage(header_r, enc::MONO8, right).toImageMsg();
    camera_info_prt->header.stamp = msg_r->header.stamp;
    m_right_image_publisher.publish(
        msg_r, sensor_msgs::CameraInfoPtr(camera_info_prt));
  }

  void onInit() override {
    nh_ = getMTNodeHandle();
    private_nh_ = getMTPrivateNodeHandle();
    image_transport::ImageTransport it_indemind(nh_);

    m_left_image_publisher =
        it_indemind.advertiseCamera("/indemind/image/left", 1, true);
    m_right_image_publisher =
        it_indemind.advertiseCamera("/indemind/image/right", 1, true);
    m_disparity_publisher =
        it_indemind.advertiseCamera("/indemind/disparity", 1, true);
    m_depth_publisher = it_indemind.advertiseCamera("/indemind/depth", 1, true);

    m_pointcloud_publisher =
        nh_.advertise<sensor_msgs::PointCloud2>("/indemind/points", 1, true);

    if (m_pSDK) {
      std::cout << "Already Exist" << std::endl;
    } else if (!m_pSDK) {
      m_pSDK = new CIMRSDK();
      MRCONFIG config = {0};
      config.bSlam = false;
      int index;
      nh_.getParam("/indemind_node/resolution_index", index);

      nh_.getParam("/indemind_node/framerate", config.imgFrequency);

      nh_.getParam("/indemind_node/imu_frequency", config.imuFrequency);

      switch (index) {
      case 1:
        config.imgResolution = IMG_640;
        break;
      case 2:
        config.imgResolution = IMG_1280;
        break;
      default:
        config.imgResolution = IMG_DEFAULT;
        break;
      }
      m_pSDK->Init(config);

      // m_pSDK->RegistModuleCameraCallback(SdkCameraCallBack, NULL);

      g_params = m_pSDK->GetModuleParams();
      LoadCameraInfo(g_params._left_camera[indem::RESOLUTION::RES_640X400]);
      m_pSDK->RegistImgCallback([&](double time, cv::Mat left, cv::Mat right) {
        if (!left.empty() && !right.empty()) {
          publishImages(time, left, right);
        }
      });
      // LoadCameraInfo(g_params._left_camera[indem::RESOLUTION::RES_640X400]);

      /*
            if (m_pSDK->EnablePointProcessor()) {
              m_pSDK->RegistRectifiedImgCallback(SdkRectifiedImgCallBack);
              m_pSDK->RegistDisparityCallback(SdkDisparityCallBack);
              m_pSDK->RegistDepthCallback(SdkDepthCallBack);
              m_pSDK->RegistPointCloudCallback(SdkPointCloudCallBack);
            }
      */
    }
  }

private:
  CIMRSDK *m_pSDK;
  MoudleAllParam g_params;
  ros::NodeHandle nh_;
  ros::NodeHandle private_nh_;
  ros::Publisher m_detector_publisher;
  image_transport::CameraPublisher m_left_image_publisher;
  image_transport::CameraPublisher m_right_image_publisher;
  image_transport::CameraPublisher m_disparity_publisher;
  image_transport::CameraPublisher m_depth_publisher;
  ros::Publisher m_pointcloud_publisher;
  sensor_msgs::CameraInfoPtr camera_info_prt;
};
} // namespace imsee

#include <pluginlib/class_list_macros.h> // NOLINT
PLUGINLIB_EXPORT_CLASS(imsee::ROSWrapperNodelet, nodelet::Nodelet);