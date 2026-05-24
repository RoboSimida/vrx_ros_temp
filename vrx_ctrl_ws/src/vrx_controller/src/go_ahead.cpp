#include <chrono>
#include <functional>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64.hpp"

using namespace std::chrono_literals;

/* This example creates a subclass of Node and uses std::bind() to register a
* member function as a callback from the timer. */

class GoAhead : public rclcpp::Node
{
  public:
    GoAhead()
    : Node("GoAhead")
    {
      thR_publisher = this->create_publisher<std_msgs::msg::Float64>("/wamv/thrusters/right/thrust", 10);
      posR_publisher = this->create_publisher<std_msgs::msg::Float64>("/wamv/thrusters/right/pos", 10);
      thL_publisher = this->create_publisher<std_msgs::msg::Float64>("/wamv/thrusters/left/thrust", 10);
      posL_publisher = this->create_publisher<std_msgs::msg::Float64>("/wamv/thrusters/left/pos", 10);
      timer_ = this->create_wall_timer(
      5000ms, std::bind(&GoAhead::timer_callback, this));
      
      this->declare_parameter<double>("N",10000);
    }

  private:
    void timer_callback()
    {
      auto message_thR = std_msgs::msg::Float64();
      auto message_posR = std_msgs::msg::Float64();
      auto message_thL = std_msgs::msg::Float64();
      auto message_posL = std_msgs::msg::Float64();
      
      double N;
      this->get_parameter<double>("N",N);

      message_thR.data = N;
      message_posR.data = 0;
      message_thL.data = N;
      message_posL.data = 0;

      RCLCPP_INFO_STREAM(this->get_logger(), "Publishing: thR posR thL posL:"
                        << message_thR.data << " " << message_posR.data << " "
                        << message_thL.data << " " << message_posL.data << " ");
      thR_publisher -> publish(message_thR);
      posR_publisher -> publish(message_posR);
      thL_publisher -> publish(message_thL);
      posL_publisher -> publish(message_posL);
    }
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr thR_publisher;
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr posR_publisher;
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr thL_publisher;
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr posL_publisher;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<GoAhead>());
  rclcpp::shutdown();
  return 0;
}
