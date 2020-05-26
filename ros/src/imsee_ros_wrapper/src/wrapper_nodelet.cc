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

static const double PI = 3.14159265358979323846;
static const double GRAVITY = 9.8015;

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
    if (m_left_image_publisher.getNumSubscribers() == 0 &&
        m_right_image_publisher.getNumSubscribers() == 0)
      return;

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
    m_right_image_publisher.publish(msg_r, camera_info_prt);
  }

  void publishImu(ImuData imu) {
    if (m_imu_publisher.getNumSubscribers() == 0)
      return;

    sensor_msgs::Imu msg;
    msg.header.stamp = hardTimeToSoftTime(imu.timestamp);
    msg.header.frame_id = "camera_imu_frame";

    msg.linear_acceleration.x = imu.accel[0] * GRAVITY;
    msg.linear_acceleration.y = imu.accel[1] * GRAVITY;
    msg.linear_acceleration.z = imu.accel[2] * GRAVITY;

    msg.linear_acceleration_covariance[0] = 0;
    msg.linear_acceleration_covariance[1] = 0;
    msg.linear_acceleration_covariance[2] = 0;

    msg.linear_acceleration_covariance[3] = 0;
    msg.linear_acceleration_covariance[4] = 0;
    msg.linear_acceleration_covariance[5] = 0;

    msg.linear_acceleration_covariance[6] = 0;
    msg.linear_acceleration_covariance[7] = 0;
    msg.linear_acceleration_covariance[8] = 0;

    msg.angular_velocity.x = imu.gyro[0] * PI / 180;
    msg.angular_velocity.y = imu.gyro[1] * PI / 180;
    msg.angular_velocity.z = imu.gyro[2] * PI / 180;

    msg.angular_velocity_covariance[0] = 0;
    msg.angular_velocity_covariance[1] = 0;
    msg.angular_velocity_covariance[2] = 0;

    msg.angular_velocity_covariance[3] = 0;
    msg.angular_velocity_covariance[4] = 0;
    msg.angular_velocity_covariance[5] = 0;

    msg.angular_velocity_covariance[6] = 0;
    msg.angular_velocity_covariance[7] = 0;
    msg.angular_velocity_covariance[8] = 0;

    m_imu_publisher.publish(msg);
  }

  void publishPoints(ros::Time stamp, cv::Mat points) {
    sensor_msgs::PointCloud2 msg;
    msg.header.stamp = stamp;
    msg.width = points.cols;
    msg.height = points.rows;
    msg.header.frame_id = "points_frame";
    msg.is_dense = true;

    sensor_msgs::PointCloud2Modifier modifier(msg);

    modifier.setPointCloud2Fields(4, "x", 1, sensor_msgs::PointField::FLOAT32,
                                  "y", 1, sensor_msgs::PointField::FLOAT32, "z",
                                  1, sensor_msgs::PointField::FLOAT32, "rgb", 1,
                                  sensor_msgs::PointField::FLOAT32);

    modifier.setPointCloud2FieldsByString(2, "xyz", "rgb");

    sensor_msgs::PointCloud2Iterator<float> iter_x(msg, "x");
    sensor_msgs::PointCloud2Iterator<float> iter_y(msg, "y");
    sensor_msgs::PointCloud2Iterator<float> iter_z(msg, "z");

    sensor_msgs::PointCloud2Iterator<uint8_t> iter_r(msg, "r");
    sensor_msgs::PointCloud2Iterator<uint8_t> iter_g(msg, "g");
    sensor_msgs::PointCloud2Iterator<uint8_t> iter_b(msg, "b");

    for (int m = 0; m < points.rows; m++) {
      for (int n = 0; n < points.cols; n++) {
        auto &&cloud = points.at<cv::Vec3f>(m, n);
        *iter_x = cloud[2];
        *iter_y = cloud[0];
        *iter_z = cloud[1];

        *iter_r = static_cast<uint8_t>(255);
        *iter_g = static_cast<uint8_t>(255);
        *iter_b = static_cast<uint8_t>(255);

        ++iter_x;
        ++iter_y;
        ++iter_z;
        ++iter_r;
        ++iter_g;
        ++iter_b;
      }
    }

    m_pointcloud_publisher.publish(msg);
  }

  void publishProcessorTopics() {
    if (m_depth_publisher.getNumSubscribers() > 0 &&
        !is_published_map[Processor::DEPTH]) {
      m_pSDK->EnableDepthProcessor();
      m_pSDK->RegistDepthCallback([&](double time, cv::Mat depth) {
        if (!depth.empty()) {
          std_msgs::Header header;
          header.stamp = hardTimeToSoftTime(time);
          header.frame_id = "camera_depth_frame";
          depth.convertTo(depth, CV_16U, 1000.0);
          auto &&msg =
              cv_bridge::CvImage(header, enc::MONO16, depth).toImageMsg();
          camera_info_prt->header.stamp = msg->header.stamp;
          m_depth_publisher.publish(msg, camera_info_prt);
        }
      });
      is_published_map[Processor::DEPTH] = true;
    }

    if (m_disparity_publisher.getNumSubscribers() > 0 &&
        !is_published_map[Processor::DISPARITY]) {
      m_pSDK->EnableDisparityProcessor();
      m_pSDK->RegistDisparityCallback([&](double time, cv::Mat disparity) {
        if (!disparity.empty()) {
          std_msgs::Header header;
          header.stamp = hardTimeToSoftTime(time);
          header.frame_id = "camera_disparity_frame";
          disparity.convertTo(disparity, CV_8UC1, 255. / (16 * 64));
          auto &&msg =
              cv_bridge::CvImage(header, enc::MONO8, disparity).toImageMsg();
          camera_info_prt->header.stamp = msg->header.stamp;
          m_disparity_publisher.publish(msg, camera_info_prt);
        }
      });
      is_published_map[Processor::DISPARITY] = true;
    }

    if ((m_rectified_left_image_publisher.getNumSubscribers() > 0 ||
         m_rectified_right_image_publisher.getNumSubscribers() > 0) &&
        !is_published_map[Processor::RECTIFIED]) {
      m_pSDK->EnableRectifyProcessor();
      m_pSDK->RegistRectifiedImgCallback(
          [&](double time, cv::Mat left, cv::Mat right) {
            if (!left.empty() && !right.empty()) {
              std_msgs::Header header_l, header_r;
              header_l.stamp = hardTimeToSoftTime(time);
              header_l.frame_id = "camera_rectified_left_frame";
              auto &&msg_l =
                  cv_bridge::CvImage(header_l, enc::MONO8, left).toImageMsg();
              camera_info_prt->header.stamp = msg_l->header.stamp;
              m_rectified_left_image_publisher.publish(msg_l, camera_info_prt);

              header_r.stamp = header_l.stamp;
              header_r.frame_id = "camera_rectified_right_frame";
              auto &&msg_r =
                  cv_bridge::CvImage(header_r, enc::MONO8, right).toImageMsg();
              camera_info_prt->header.stamp = msg_r->header.stamp;
              m_rectified_right_image_publisher.publish(
                  msg_r, sensor_msgs::CameraInfoPtr(camera_info_prt));
            }
          });
      is_published_map[Processor::RECTIFIED] = true;
    }

    if (m_pointcloud_publisher.getNumSubscribers() > 0 &&
        !is_published_map[Processor::POINTS]) {
      m_pSDK->EnablePointProcessor();
      m_pSDK->RegistPointCloudCallback([&](double time, cv::Mat points) {
        if (!points.empty()) {
          publishPoints(hardTimeToSoftTime(time), points);
        }
      });
      is_published_map[Processor::POINTS] = true;
    }

    if (m_detector_publisher.getNumSubscribers() > 0 &&
        !is_published_map[Processor::DETECTOR]) {
      m_pSDK->EnableDetectorProcessor();
      m_pSDK->RegistDetectorCallback([&](indem::DetectorInfo info) {
        if (!info.img.empty()) {
          std_msgs::Header header;
          header.stamp = hardTimeToSoftTime(info.timestamp);
          header.frame_id = "camera_detector_frame";
          auto &&msg =
              cv_bridge::CvImage(header, enc::BGR8, info.img).toImageMsg();
          camera_info_prt->header.stamp = msg->header.stamp;
          m_detector_publisher.publish(msg, camera_info_prt);
        }
      });
      is_published_map[Processor::DETECTOR] = true;
    }
  }

  void PublishStaticTransforms() {
    ros::Time tf_stamp = ros::Time::now();

    // The left frame is used as the base frame.
    geometry_msgs::TransformStamped b2l_msg;
    b2l_msg.header.stamp = tf_stamp;
    b2l_msg.header.frame_id = "imsee_link";
    b2l_msg.child_frame_id = "camera_left_frame";
    b2l_msg.transform.translation.x = 0;
    b2l_msg.transform.translation.y = 0;
    b2l_msg.transform.translation.z = 0;
    b2l_msg.transform.rotation.x = 0;
    b2l_msg.transform.rotation.y = 0;
    b2l_msg.transform.rotation.z = 0;
    b2l_msg.transform.rotation.w = 1;
    m_static_tf_broadcaster.sendTransform(b2l_msg);

    // Transform left frame to disparity frame
    geometry_msgs::TransformStamped l2disp_msg;
    l2disp_msg.header.stamp = tf_stamp;
    l2disp_msg.header.frame_id = "camera_left_frame";
    l2disp_msg.child_frame_id = "disparity_frame";
    l2disp_msg.transform.translation.x = 0;
    l2disp_msg.transform.translation.y = 0;
    l2disp_msg.transform.translation.z = 0;
    l2disp_msg.transform.rotation.x = 0;
    l2disp_msg.transform.rotation.y = 0;
    l2disp_msg.transform.rotation.z = 0;
    l2disp_msg.transform.rotation.w = 1;
    m_static_tf_broadcaster.sendTransform(l2disp_msg);

    // Transform left frame to depth frame
    geometry_msgs::TransformStamped b2d_msg;
    b2d_msg.header.stamp = tf_stamp;
    b2d_msg.header.frame_id = "camera_left_frame";
    b2d_msg.child_frame_id = "depth_frame";
    b2d_msg.transform.translation.x = 0;
    b2d_msg.transform.translation.y = 0;
    b2d_msg.transform.translation.z = 0;
    b2d_msg.transform.rotation.x = 0;
    b2d_msg.transform.rotation.y = 0;
    b2d_msg.transform.rotation.z = 0;
    b2d_msg.transform.rotation.w = 1;
    m_static_tf_broadcaster.sendTransform(b2d_msg);

    // Transform left frame to points frame
    geometry_msgs::TransformStamped b2p_msg;
    b2p_msg.header.stamp = tf_stamp;
    b2p_msg.header.frame_id = "camera_left_frame";
    b2p_msg.child_frame_id = "points_frame";
    b2p_msg.transform.translation.x = 0;
    b2p_msg.transform.translation.y = 0;
    b2p_msg.transform.translation.z = 0;
    b2p_msg.transform.rotation.x = 0;
    b2p_msg.transform.rotation.y = 0;
    b2p_msg.transform.rotation.z = 0;
    b2p_msg.transform.rotation.w = 1;
    m_static_tf_broadcaster.sendTransform(b2p_msg);
  }

  void onInit() override {
    nh_ = getMTNodeHandle();
    private_nh_ = getMTPrivateNodeHandle();
    image_transport::ImageTransport it_imsee(nh_);

    m_detector_publisher =
        it_imsee.advertiseCamera("/imsee/image/detector", 1, true);
    m_left_image_publisher =
        it_imsee.advertiseCamera("/imsee/image/left", 1, true);
    m_right_image_publisher =
        it_imsee.advertiseCamera("/imsee/image/right", 1, true);
    m_rectified_left_image_publisher =
        it_imsee.advertiseCamera("/imsee/image/rectified/left", 1, true);
    m_rectified_right_image_publisher =
        it_imsee.advertiseCamera("/imsee/image/rectified/right", 1, true);
    m_disparity_publisher =
        it_imsee.advertiseCamera("/imsee/disparity", 1, true);
    m_depth_publisher = it_imsee.advertiseCamera("/imsee/depth", 1, true);

    m_pointcloud_publisher =
        nh_.advertise<sensor_msgs::PointCloud2>("/imsee/points", 1, true);
    m_imu_publisher = nh_.advertise<sensor_msgs::Imu>("/imsee/imu", 100);
    if (m_pSDK) {
      std::cout << "Already Exist" << std::endl;
    } else if (!m_pSDK) {
      m_pSDK = new CIMRSDK();
      MRCONFIG config = {0};
      RESOLUTION res;
      config.bSlam = false;
      int index;

      nh_.getParam("/indemind_node/resolution_index", index);
      nh_.getParam("/indemind_node/framerate", config.imgFrequency);
      nh_.getParam("/indemind_node/imu_frequency", config.imuFrequency);

      switch (index) {
      case 1:
        config.imgResolution = IMG_640;
        res = RESOLUTION::RES_640X400;
        break;
      case 2:
        config.imgResolution = IMG_1280;
        res = RESOLUTION::RES_1280X800;
        break;
      default:
        config.imgResolution = IMG_DEFAULT;
        res = RESOLUTION::RES_640X400;
        break;
      }
      m_pSDK->Init(config);

      PublishStaticTransforms();
      // m_pSDK->RegistModuleCameraCallback(SdkCameraCallBack, NULL);

      g_params = m_pSDK->GetModuleParams();
      LoadCameraInfo(g_params._left_camera[res]);
      m_pSDK->RegistImgCallback([&](double time, cv::Mat left, cv::Mat right) {
        if (!left.empty() && !right.empty()) {
          publishImages(time, left, right);
        }
      });
      m_pSDK->RegistModuleIMUCallback([&](ImuData imu) { publishImu(imu); });
      ros::Rate loop_rate(config.imgFrequency);
      while (private_nh_.ok()) {
        publishProcessorTopics();
        loop_rate.sleep();
      }

      // m_pSDK->RegistDetectorCallback([&](indem::DetectorInfo info) {});

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

  enum Processor { RECTIFIED, DISPARITY, DEPTH, POINTS, DETECTOR };
  std::map<Processor, bool> is_published_map{{Processor::RECTIFIED, false},
                                             {Processor::DISPARITY, false},
                                             {Processor::DEPTH, false},
                                             {Processor::POINTS, false},
                                             {Processor::DETECTOR, false}};

  ros::NodeHandle nh_;
  ros::NodeHandle private_nh_;
  image_transport::CameraPublisher m_detector_publisher;
  image_transport::CameraPublisher m_left_image_publisher;
  image_transport::CameraPublisher m_right_image_publisher;
  image_transport::CameraPublisher m_rectified_left_image_publisher;
  image_transport::CameraPublisher m_rectified_right_image_publisher;
  image_transport::CameraPublisher m_disparity_publisher;
  image_transport::CameraPublisher m_depth_publisher;
  ros::Publisher m_pointcloud_publisher;
  ros::Publisher m_imu_publisher;
  tf2_ros::StaticTransformBroadcaster m_static_tf_broadcaster;
  sensor_msgs::CameraInfoPtr camera_info_prt;
};
} // namespace imsee

#include <pluginlib/class_list_macros.h> // NOLINT
PLUGINLIB_EXPORT_CLASS(imsee::ROSWrapperNodelet, nodelet::Nodelet);