#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <cmath>

#include "rclcpp/rclcpp.hpp"
#include <sensor_msgs/msg/imu.hpp>
#include <vrx_other_interface/msg/state.hpp>

using namespace std::placeholders;


class fuse_sunstitue : public rclcpp::Node
{
  public:
    fuse_sunstitue()
    : Node("fuse_sunstitue")
    {
      imu_sub = this->create_subscription<sensor_msgs::msg::Imu>(
      "/wamv/sensors/imu/imu/data", 10, std::bind(&fuse_sunstitue::IMU_Callback, this, _1));

      state_publisher = this->create_publisher<vrx_other_interface::msg::State>("/state", 10);

      cnt = 0;
      time = 0;
      x = 0; y = 0; psi = 0;
      u = 0; v = 0; last_r = 0;
    }

  private:
    double average(double* p)
    {
      double sum = 0;
      for(int i=0; i<20; i++)
        sum += *(p+i);
      return sum/20;
    }

    void IMU_Callback(const sensor_msgs::msg::Imu& msg)
    {
        auto t = msg.header.stamp;
        double sec = t.sec + t.nanosec/1000000000.0;
        double dt = sec- time;

        double au = msg.linear_acceleration.x;
        double av = msg.linear_acceleration.y;
        double r = msg.angular_velocity.z;

        cnt++;
        r_lis[cnt%10] = r;
        r = average(r_lis);

        time = sec;
        double acc_r = (r-last_r)/dt;
        last_r = r;
        u += au * dt;
        v += av * dt;

        psi += r * dt;
        double cosine = std::cos(psi);
        double sine = std::sin(psi);
        x += (u * cosine - v * sine)*dt;
        y += (u * sine + v * cosine)*dt;

        auto message = vrx_other_interface::msg::State();
        message.time = time;
        message.pos.x = x;
        message.pos.y = y;
        message.pos.psi = psi;
        message.vel.u = u;
        message.vel.v = v;
        message.vel.psi = r;
        message.acc.u = au;
        message.acc.v = av;
        message.acc.psi = acc_r;

        state_publisher->publish(message);
    }

    rclcpp::Subscription<sensor_msgs::msg::Imu>::SharedPtr imu_sub;
    rclcpp::Publisher<vrx_other_interface::msg::State>::SharedPtr state_publisher;

    double time;
    double x; double y; double psi;
    double u; double v; double last_r;
    int cnt;
    double r_lis[20] = {0, 0 ,0 ,0 ,0, 0, 0, 0, 0, 0,
    			0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<fuse_sunstitue>());
  rclcpp::shutdown();
  return 0;
}


