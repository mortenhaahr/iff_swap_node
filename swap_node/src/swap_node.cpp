#include <chrono>
#include <functional>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/bool.hpp"
#include "std_msgs/msg/string.hpp"

using std::placeholders::_1;

class SwapNode : public rclcpp::Node {
public:
  SwapNode() : Node("switch_forwarder"), use_yolo(false) {
    // Subscribers
    intel_sub = this->create_subscription<std_msgs::msg::String>(
        "intel_data", 10, std::bind(&SwapNode::intel_callback, this, _1));

    yolo_sub = this->create_subscription<std_msgs::msg::String>(
        "yolo_data", 10, std::bind(&SwapNode::yolo_callback, this, _1));

    switch_sub_ = this->create_subscription<std_msgs::msg::Bool>(
        "use_yolo", 10, std::bind(&SwapNode::use_yolo_callback, this, _1));

    // Publisher
    pub_output_ = this->create_publisher<std_msgs::msg::String>("output", 10);

    RCLCPP_INFO(this->get_logger(), "SwapNode node started.");
  }

private:
  void intel_callback(const std_msgs::msg::String::SharedPtr msg) {
    if (!use_yolo) {
      pub_output_->publish(*msg);
    }
  }

  void yolo_callback(const std_msgs::msg::String::SharedPtr msg) {
    if (use_yolo) {
      pub_output_->publish(*msg);
    }
  }

  void use_yolo_callback(const std_msgs::msg::Bool::SharedPtr msg) {
    use_yolo = msg->data;
    RCLCPP_INFO(this->get_logger(), "Switched to %s stream",
                use_yolo ? "data2" : "data1");
  }

  // Subscribers
  rclcpp::Subscription<std_msgs::msg::String>::SharedPtr intel_sub;
  rclcpp::Subscription<std_msgs::msg::String>::SharedPtr yolo_sub;
  rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr switch_sub_;

  // Publisher
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr pub_output_;

  // Internal state
  bool use_yolo;
};

int main(int argc, char *argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<SwapNode>());
  rclcpp::shutdown();
  return 0;
}
