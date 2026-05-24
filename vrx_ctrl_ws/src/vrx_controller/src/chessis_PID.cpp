#include <chrono>
#include <functional>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64.hpp"
#include "sensor_msgs/msg/joint_state.hpp"

#define Pi  3.14159265358979323846


using namespace std::chrono_literals;
using namespace std::placeholders;


class ch_PID : public rclcpp::Node
{
  public:
    ch_PID() : Node("ch_PID")
    {
      posR_publisher = this->create_publisher<std_msgs::msg::Float64>("/wamv/thrusters/right/pos", 10);
      posL_publisher = this->create_publisher<std_msgs::msg::Float64>("/wamv/thrusters/left/pos", 10);

      sub = this->create_subscription<std_msgs::msg::Float64>(
      "/Chessis_Input", 10, std::bind(&ch_PID::InputU, this, _1));

      feedback_sub = this->create_subscription<sensor_msgs::msg::JointState>(
      "/wamv/joint_states", 10, std::bind(&ch_PID::Control, this, _1));

      P = 5; I = 4; D = 1;
      Maximum = Pi/12; Minimum = -Pi/12;
      Integtation = 0;
      t = 0;
      e_last = 0;
    }

  private:
    void Control(const sensor_msgs::msg::JointState& msg)
    {
        float sec = (float)msg.header.stamp.sec;
        float nanosec = (float)msg.header.stamp.nanosec;
        float time = sec + nanosec/1e9;
        float dt = time - t;
        t = time;

        double feedback = msg.position[4];
        double e = Chessis_Input - feedback;
        Integtation += e*dt;
        double Deviation = (e-e_last)/dt;
        e = e_last;
        double u = P*e + I*Integtation + D*Deviation;

        auto pub_msg = std_msgs::msg::Float64();
        pub_msg.data = u;
        posL_publisher -> publish(pub_msg);
        posR_publisher -> publish(pub_msg);
    }

    void InputU(const std_msgs::msg::Float64& msg)
    {
        Chessis_Input = saturate(msg.data);

    }

    double saturate(double num)
    {
        if(num < Minimum)
            return Minimum;
        else if(num > Maximum)
            return Maximum;
        else
            return num;
    }

    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr posR_publisher;
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr posL_publisher;

    rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr sub;
    rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr feedback_sub;

    double P;
    double I;
    double D;
    double Maximum;
    double Minimum;
    double Chessis_Input;
    double Integtation;
    float t;
    double e_last;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<ch_PID>());
  rclcpp::shutdown();
  return 0;
}
