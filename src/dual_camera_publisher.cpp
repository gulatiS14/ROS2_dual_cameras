#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <cv_bridge/cv_bridge.h>
#include <image_transport/image_transport.h>
#include <opencv2/opencv.hpp>

class DualCameraPublisher : public rclcpp::Node
{
public:
  DualCameraPublisher()
  : Node("dual_camera_publisher")
  {
    left_publisher_ = image_transport::create_publisher(this, "left_camera/image_raw");
    right_publisher_ = image_transport::create_publisher(this, "right_camera/image_raw");

    left_cap_.open(0);  // Adjust the device number if needed
    right_cap_.open(2); // Adjust the device number if needed

    if (!left_cap_.isOpened() || !right_cap_.isOpened()) {
      RCLCPP_ERROR(this->get_logger(), "Failed to open one or both cameras");
      rclcpp::shutdown();
    }

    timer_ = this->create_wall_timer(
      std::chrono::milliseconds(33), // ~30 fps
      std::bind(&DualCameraPublisher::publishImages, this));
  }

private:
  void publishImages()
  {
    cv::Mat left_frame, right_frame;
    left_cap_ >> left_frame;
    right_cap_ >> right_frame;

    if (left_frame.empty() || right_frame.empty()) {
      RCLCPP_WARN(this->get_logger(), "Received empty frame(s)");
      return;
    }

    sensor_msgs::msg::Image::SharedPtr left_msg = cv_bridge::CvImage(std_msgs::msg::Header(), "bgr8", left_frame).toImageMsg();
    sensor_msgs::msg::Image::SharedPtr right_msg = cv_bridge::CvImage(std_msgs::msg::Header(), "bgr8", right_frame).toImageMsg();

    left_publisher_.publish(left_msg);
    right_publisher_.publish(right_msg);
  }

  image_transport::Publisher left_publisher_;
  image_transport::Publisher right_publisher_;
  cv::VideoCapture left_cap_;
  cv::VideoCapture right_cap_;
  rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<DualCameraPublisher>());
  rclcpp::shutdown();
  return 0;
}