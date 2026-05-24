#include <chrono>
#include <functional>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64.hpp"

#define Pi  3.14159265358979323846

using namespace std::chrono_literals;

/* This example creates a subclass of Node and uses std::bind() to register a
* member function as a callback from the timer. */

class Measure_Joint_T : public rclcpp::Node
{
  public:
    Measure_Joint_T()
    : Node("Measure_Joint_T")
    {
      thR_publisher = this->create_publisher<std_msgs::msg::Float64>("/wamv/thrusters/right/thrust", 10);
      posR_publisher = this->create_publisher<std_msgs::msg::Float64>("/wamv/thrusters/right/pos", 10);
      thL_publisher = this->create_publisher<std_msgs::msg::Float64>("/wamv/thrusters/left/thrust", 10);
      posL_publisher = this->create_publisher<std_msgs::msg::Float64>("/wamv/thrusters/left/pos", 10);
      timer_ = this->create_wall_timer(
      1ms, std::bind(&Measure_Joint_T::timer_callback, this));
      
      this->declare_parameter<double>("N",1000);
      this->declare_parameter<double>("Ang",Pi/2);
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
      double ang;
      this->get_parameter<double>("Ang",ang);

      message_thR.data = N;
      message_posR.data = ang;
      message_thL.data = N;
      message_posL.data = ang;

      // RCLCPP_INFO_STREAM(this->get_logger(), "Publishing: thR posR thL posL:"
      //                   << message_thR.data << " " << message_posR.data << " "
      //                   << message_thL.data << " " << message_posL.data << " ");
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
  rclcpp::spin(std::make_shared<Measure_Joint_T>());
  rclcpp::shutdown();
  return 0;
}
