// Copyright 2020 Indemind Co., Ltd. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include "imrdata.h"
#include "imrsdk.h"
#include "logging.h"
#include "types.h"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <queue>

using namespace indem;

static cv::Mat cv_in_left, cv_in_left_inv;
class DepthRegion {
public:
  explicit DepthRegion(std::uint32_t n)
      : n_(std::move(n)), show_(false), selected_(false), point_(0, 0) {}

  ~DepthRegion() = default;

  /**
   * 鼠标事件：默认不选中区域，随鼠标移动而显示。单击后，则会选中区域来显示。你可以再单击已选中区域或双击未选中区域，取消选中。
   */
  void OnMouse(const int &event, const int &x, const int &y, const int &flags) {
    if (event != cv::EVENT_MOUSEMOVE && event != cv::EVENT_LBUTTONDOWN) {
      return;
    }
    show_ = true;

    if (event == cv::EVENT_MOUSEMOVE) {
      if (!selected_) {
        point_.x = x;
        point_.y = y;
      }
    } else if (event == cv::EVENT_LBUTTONDOWN) {
      if (selected_) {
        if (x >= static_cast<int>(point_.x - n_) &&
            x <= static_cast<int>(point_.x + n_) &&
            y >= static_cast<int>(point_.y - n_) &&
            y <= static_cast<int>(point_.y + n_)) {
          selected_ = false;
        }
      } else {
        selected_ = true;
      }
      point_.x = x;
      point_.y = y;
    }
  }

  template <typename T>
  void ShowElems(const cv::Mat &depth,
                 std::function<std::string(const T &elem)> elem2string,
                 int elem_space = 60,
                 std::function<std::string(
                     const cv::Mat &depth, const cv::Point &point,
                     const std::uint32_t &n, double X, double Y, double Z)>
                     getinfo = nullptr) {
    if (!show_)
      return;

    int space = std::move(elem_space);
    int n = 2 * n_ + 1;
    cv::Mat im(space * n, space * n, CV_8UC3, cv::Scalar(255, 255, 255));

    int x, y;
    std::string str;
    int baseline = 0;

    // calculate (X, Y, Z) in left camera coordinate
    cv::Mat mouse_left_cor(3, 1, CV_64FC1), mouse_img_cor(3, 1, CV_64FC1);
    mouse_img_cor.at<double>(0, 0) = static_cast<double>(point_.x);
    mouse_img_cor.at<double>(0, 1) = static_cast<double>(point_.y);
    mouse_img_cor.at<double>(0, 2) = 1.0;
    double Z = depth.at<T>(point_.y, point_.x);
    mouse_left_cor = cv_in_left_inv * Z * mouse_img_cor;
    // std::cout << std::endl << "Mouse Left Cor:" << std::endl
    //   << mouse_left_cor << std::endl;

    for (int i = -n_; i <= n; ++i) {
      x = point_.x + i;
      if (x < 0 || x >= depth.cols)
        continue;
      for (int j = -n_; j <= n; ++j) {
        y = point_.y + j;
        if (y < 0 || y >= depth.rows)
          continue;

        str = elem2string(depth.at<T>(y, x));

        cv::Scalar color(0, 0, 0);
        if (i == 0 && j == 0)
          color = cv::Scalar(0, 0, 255);

        cv::Size sz =
            cv::getTextSize(str, cv::FONT_HERSHEY_PLAIN, 1, 1, &baseline);

        cv::putText(im, str,
                    cv::Point((i + n_) * space + (space - sz.width) / 2,
                              (j + n_) * space + (space + sz.height) / 2),
                    cv::FONT_HERSHEY_PLAIN, 1, color, 1);
      }
    }

    if (getinfo) {
      double x, y, z;
      x = mouse_left_cor.at<double>(0, 0);
      y = mouse_left_cor.at<double>(1, 0);
      z = mouse_left_cor.at<double>(2, 0);
      std::string info = getinfo(depth, point_, n_, x, y, z);
      if (!info.empty()) {
        cv::Size sz =
            cv::getTextSize(info, cv::FONT_HERSHEY_PLAIN, 1, 1, &baseline);

        cv::putText(im, info, cv::Point(5, 5 + sz.height),
                    cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(255, 0, 255), 1);
      }
    }

    cv::imshow("region", im);
  }

  void DrawRect(cv::Mat &image) { // NOLINT
    if (!show_)
      return;
    std::uint32_t n = (n_ > 1) ? n_ : 1;
    n += 1; // outside the region
    cv::rectangle(image, cv::Point(point_.x - n, point_.y - n),
                  cv::Point(point_.x + n, point_.y + n),
                  selected_ ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 255), 1);
  }

private:
  std::uint32_t n_;
  bool show_;
  bool selected_;
  cv::Point point_;
};
void OnDepthMouseCallback(int event, int x, int y, int flags, void *userdata) {
  DepthRegion *region = reinterpret_cast<DepthRegion *>(userdata);
  region->OnMouse(event, x, y, flags);
}

template <typename T> void clear(std::queue<T> &q) {
  std::queue<T> empty;
  swap(empty, q);
}

int main(int argc, char **argv) {
  auto m_pSDK = new CIMRSDK();
  MRCONFIG config = {0};
  config.bSlam = false;
  config.imgResolution = IMG_640;
  config.imgFrequency = 50;
  config.imuFrequency = 1000;

  m_pSDK->Init(config);

  auto param = m_pSDK->GetModuleParams()._left_camera[RESOLUTION::RES_640X400];
  cv_in_left = cv::Mat::eye(3, 3, CV_64F);
  cv_in_left.at<double>(0, 0) = param._K[0];
  cv_in_left.at<double>(1, 1) = param._K[4];
  cv_in_left.at<double>(0, 2) = param._K[2];
  cv_in_left.at<double>(1, 2) = param._K[5];
  cv_in_left_inv = cv_in_left.inv();
  DepthRegion depth_region(3);
  auto depth_info = [](const cv::Mat &depth, const cv::Point &point,
                       const std::uint32_t &n, double X, double Y, double Z) {
    std::ostringstream os;
    os << "depth pos(" << n << "): [" << point.y << ", " << point.x << "]"
       << " camera pos: [" << X << ", " << Y << ", " << Z << "]"
       << ", unit: mm";

    return os.str();
  };
  std::queue<cv::Mat> disparity_queue, depth_queue;
  if (m_pSDK->EnableDepthProcessor()) {
    m_pSDK->RegistDepthCallback([&depth_queue](double time, cv::Mat depth) {
      if (!depth.empty()) {
        depth.convertTo(depth, CV_16U, 1000.0);
        depth_queue.push(depth);
      }
    });
  }
  if (m_pSDK->EnableDisparityProcessor()) {
    m_pSDK->RegistDisparityCallback(
        [&disparity_queue](double time, cv::Mat disparity) {
          if (!disparity.empty()) {
            disparity.convertTo(disparity, CV_8U, 255. / (16 * 64));
            cv::applyColorMap(disparity, disparity, cv::COLORMAP_JET);
            disparity.setTo(0, disparity == -16);
            disparity_queue.push(disparity);
          }
        });
  }

  while (true) {
    if (!depth_queue.empty() && !disparity_queue.empty()) {
      cv::namedWindow("depth");
      cv::imshow("depth", disparity_queue.front());
      cv::setMouseCallback("depth", OnDepthMouseCallback, &depth_region);
      // Note: DrawRect will change some depth values to show the rect.
      depth_region.DrawRect(depth_queue.front());
      depth_region.ShowElems<ushort>(
          depth_queue.front(),
          [](const ushort &elem) {
            if (elem >= 10000) {
              return std::string("invalid");
            }
            return std::to_string(elem);
          },
          90, depth_info);
      clear(disparity_queue);
      clear(depth_queue);
    }
    char key = static_cast<char>(cv::waitKey(1));
    if (key == 27 || key == 'q' || key == 'Q') { // ESC/Q
      break;
    }
  }
  delete m_pSDK;
  return 0;
}