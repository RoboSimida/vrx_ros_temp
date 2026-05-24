#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <cstdlib>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64.hpp"
#include <sensor_msgs/msg/nav_sat_fix.hpp>

#define Pi  3.14159265

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
      clock_sub = this->create_subscription<sensor_msgs::msg::NavSatFix>(
       "/wamv/sensors/gps/gps/fix", 10, std::bind(&SysIden::Clock_Callback, this, _1));
       
      this->declare_parameter<float>("Ang",Pi/12);
      this->get_parameter<float>("Ang",Ang_max);
      
      cnt = 0;
      N = 71;
      Ang = 0;
      N_max = 71;
      N_min = 0;
      Ang_min = -Ang_max;
    }

  private:
    void timer_callback()
    {
      // if(cnt % 10 == 0)
      // 	Ang = (std::rand() % ((Ang_max - Ang_min) + 1))/10000.0 + N_min/10000.0;
      // N = (std::rand() % ((N_max - N_min)*100 + 1))/100.0 + N_min;
      
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

    }


    void Clock_Callback(const sensor_msgs::msg::NavSatFix& msg)
    {
      int sec = msg.header.stamp.sec;
      int nanosec = msg.header.stamp.nanosec;
      double t = sec + nanosec/1e9;

      if(t / 5 > cnt)
      {
        cnt ++;
        if(cnt % 2 == 0)
        {
            Ang = Ang_min;
            N = N_min;
        }
        else
        {
            Ang = Ang_max;
            N = N_max;
        }
      }
      timer_callback();
      RCLCPP_INFO_STREAM(this->get_logger(),"\ncnt: " << cnt );
    }

    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr thR_publisher;
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr posR_publisher;
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr thL_publisher;
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr posL_publisher;

    rclcpp::Subscription<sensor_msgs::msg::NavSatFix>::SharedPtr clock_sub;


    int cnt;
    double N;
    double Ang;
    float N_max;
    float N_min;
    float Ang_max;
    float Ang_min;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<SysIden>());
  rclcpp::shutdown();
  return 0;
}
