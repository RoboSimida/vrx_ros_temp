#include <chrono>
#include <functional>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64.hpp"

#define Pi  3.14159265358979323846

using namespace std::placeholders;
using namespace std::chrono_literals;


class step_speed_up : public rclcpp::Node
{
  public:
    step_speed_up()
    : Node("step_speed_up")
    {
      thR_publisher = this->create_publisher<std_msgs::msg::Float64>("/wamv/thrusters/right/thrust", 10);
      posR_publisher = this->create_publisher<std_msgs::msg::Float64>("/wamv/thrusters/right/pos", 10);
      thL_publisher = this->create_publisher<std_msgs::msg::Float64>("/wamv/thrusters/left/thrust", 10);
      posL_publisher = this->create_publisher<std_msgs::msg::Float64>("/wamv/thrusters/left/pos", 10);
      
      timer_ = this->create_wall_timer(
      1ms, std::bind(&step_speed_up::timer_callback, this));
    }

  private:
    void timer_callback()
    {
      auto message_thR = std_msgs::msg::Float64();
      auto message_posR = std_msgs::msg::Float64();
      auto message_thL = std_msgs::msg::Float64();
      auto message_posL = std_msgs::msg::Float64();


      message_thR.data = 100;
      message_posR.data = Pi/3;
      message_thL.data = 100;
      message_posL.data = Pi/3;

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

    int cnt;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<step_speed_up>());
  rclcpp::shutdown();
  return 0;
}
