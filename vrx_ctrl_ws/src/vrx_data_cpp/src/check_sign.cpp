#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <cmath>

#include "rclcpp/rclcpp.hpp"
#include <sensor_msgs/msg/imu.hpp>
#include <vrx_other_interface/msg/state.hpp>


using namespace std::chrono_literals;
using namespace std::placeholders;


class check_sign : public rclcpp::Node
{
  public:
    check_sign()
    : Node("check_sign")
    {
      state_sub = this->create_subscription<vrx_other_interface::msg::State>(
      "/state", 10, std::bind(&check_sign::State_Callback, this, _1));
      imu_sub = this->create_subscription<sensor_msgs::msg::Imu>(
      "/wamv/sensors/imu/imu/data", 10, std::bind(&check_sign::IMU_Callback, this, _1));

      timer = this->create_wall_timer(500ms, std::bind(&check_sign::Callback, this));

      x = 0; y = 0; psi = 0;
      u = 0; v = 0; r = 0;
      imu_u = 0; imu_v = 0; imu_r = 0;
    }

  private:
    void State_Callback(const vrx_other_interface::msg::State& msg)
    {
        x = msg.pos.x;
        y = msg.pos.y;
        psi = msg.pos.psi;
        u = msg.vel.u;
        v = msg.vel.v;
        r = msg.vel.psi;
    }
    
    void IMU_Callback(const sensor_msgs::msg::Imu& msg)
    {
    	imu_u = msg.linear_acceleration.x;
    	imu_v = msg.linear_acceleration.y;
    	imu_r = msg.angular_velocity.z;
        
    }

    void Callback()
    {
        RCLCPP_INFO_STREAM(this->get_logger(),
        "\n" << "x: " << x << "  y: " << y << "  p: " << psi << 
        "\n" << "u: " << u << "  v: " << v << "  r: " << r <<
        "\n" << "iau:" << imu_u << " iav: " << imu_v <<
        " ir: " << imu_r << "\n");
    }



    rclcpp::Subscription<vrx_other_interface::msg::State>::SharedPtr state_sub;
    rclcpp::Subscription<sensor_msgs::msg::Imu>::SharedPtr imu_sub;
    rclcpp::TimerBase::SharedPtr timer;

    double x;
    double y;
    double psi;
    double u;
    double v;
    double r;
    double imu_u;
    double imu_v;
    double imu_r;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<check_sign>());
  rclcpp::shutdown();
  return 0;
}


