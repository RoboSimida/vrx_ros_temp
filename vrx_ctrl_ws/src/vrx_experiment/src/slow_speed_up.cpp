#include <chrono>
#include <functional>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64.hpp"
#include "rosgraph_msgs/msg/clock.hpp"

#define Pi  3.14159265358979323846

using namespace std::placeholders;
using namespace std::chrono_literals;

/* This example creates a subclass of Node and uses std::bind() to register a
* member function as a callback from the timer. */

class slow_speed_up : public rclcpp::Node
{
  public:
    slow_speed_up()
    : Node("slow_speed_up")
    {
      thR_publisher = this->create_publisher<std_msgs::msg::Float64>("/wamv/thrusters/right/thrust", 10);
      posR_publisher = this->create_publisher<std_msgs::msg::Float64>("/wamv/thrusters/right/pos", 10);
      thL_publisher = this->create_publisher<std_msgs::msg::Float64>("/wamv/thrusters/left/thrust", 10);
      posL_publisher = this->create_publisher<std_msgs::msg::Float64>("/wamv/thrusters/left/pos", 10);
      timer_ = this->create_wall_timer(
      1ms, std::bind(&slow_speed_up::timer_callback, this));

      clock_sub = this->create_subscription<rosgraph_msgs::msg::Clock>(
      "/clock", 10, std::bind(&slow_speed_up::Clock_Callback, this, _1));

      speed = 0;
    }

  private:
    void timer_callback()
    {
      auto message_thR = std_msgs::msg::Float64();
      auto message_posR = std_msgs::msg::Float64();
      auto message_thL = std_msgs::msg::Float64();
      auto message_posL = std_msgs::msg::Float64();


      message_thR.data = speed;
      message_posR.data = 0;
      message_thL.data = speed;
      message_posL.data = 0;

      thR_publisher -> publish(message_thR);
      posR_publisher -> publish(message_posR);
      thL_publisher -> publish(message_thL);
      posL_publisher -> publish(message_posL);
    }

    void Clock_Callback(const rosgraph_msgs::msg::Clock& msg)
    {
      int sec = msg.clock.sec;
      int nanosec = msg.clock.nanosec;
      double t = sec + nanosec/1000000000.0;
      speed = t*25;
    }

    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr thR_publisher;
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr posR_publisher;
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr thL_publisher;
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr posL_publisher;
    rclcpp::Subscription<rosgraph_msgs::msg::Clock>::SharedPtr clock_sub;
    
    double speed;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<slow_speed_up>());
  rclcpp::shutdown();
  return 0;
}
