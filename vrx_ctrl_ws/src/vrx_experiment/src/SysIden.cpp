#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <cstdlib>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64.hpp"
// #include "rosgraph_msgs/msg/clock.hpp"

using namespace std::chrono_literals;
using namespace std::placeholders;

class SysIden : public rclcpp::Node
{
  public:
    SysIden()
    : Node("SysIden")
    {
      thR_publisher = this->create_publisher<std_msgs::msg::Float64>("/wamv/thrusters/right/thrust", 10);
      posR_publisher = this->create_publisher<std_msgs::msg::Float64>("/wamv/thrusters/right/pos", 10);
      thL_publisher = this->create_publisher<std_msgs::msg::Float64>("/wamv/thrusters/left/thrust", 10);
      posL_publisher = this->create_publisher<std_msgs::msg::Float64>("/wamv/thrusters/left/pos", 10);
	
      timer_ = this->create_wall_timer(
      5000ms, std::bind(&SysIden::timer_callback, this));	
      // clock_sub = this->create_subscription<rosgraph_msgs::msg::Clock>(
      // "/clock", 10, std::bind(&SysIden::Clock_Callback, this, _1));

      cnt = 0;
      N = 0;
      Ang = 0;
      N_max = 71;
      N_min = 0;
      Ang_max = 7854;  // Pi/4*10000
      Ang_min = -7854;
    }

  private:
    void timer_callback()
    {
    
      if(cnt % 10 == 0)
      	Ang = (std::rand() % ((Ang_max - Ang_min) + 1))/10000.0 + N_min/10000.0;
      N = 100;// N = (std::rand() % ((N_max - N_min)*100 + 1))/100.0 + N_min;
      auto message_thR = std_msgs::msg::Float64();
      auto message_posR = std_msgs::msg::Float64();
      auto message_thL = std_msgs::msg::Float64();
      auto message_posL = std_msgs::msg::Float64();

      message_thR.data = N;
      message_posR.data = Ang;
      message_thL.data = N;
      message_posL.data = Ang;

      thR_publisher -> publish(message_thR);
      posR_publisher -> publish(message_posR);
      thL_publisher -> publish(message_thL);
      posL_publisher -> publish(message_posL);
      
      cnt ++;
    }

    /*
    void Clock_Callback(const rosgraph_msgs::msg::Clock& msg)
    {
      int sec = msg.clock.sec;
      int nanosec = msg.clock.nanosec;
      double t = sec + nanosec/1000000000.0;

      if(t / 0.02 > cnt)
      {
        cnt ++;
        if(cnt % 10 == 0)
            Ang = (std::rand() % ((Ang_max - Ang_min) + 1))/10000.0 + Ang_min/10000.0;
        N = (std::rand() % ((N_max - N_min)*100 + 1))/100.0 + N_min;
        timer_callback();
      }
    } */

    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr thR_publisher;
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr posR_publisher;
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr thL_publisher;
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr posL_publisher;

    // rclcpp::Subscription<rosgraph_msgs::msg::Clock>::SharedPtr clock_sub;


    int cnt;
    double N;
    double Ang;
    int N_max;
    int N_min;
    int Ang_max;
    int Ang_min;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<SysIden>());
  rclcpp::shutdown();
  return 0;
}
