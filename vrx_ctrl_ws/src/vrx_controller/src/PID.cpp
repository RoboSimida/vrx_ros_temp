#include <chrono>
#include <functional>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64.hpp"
#include <vrx_other_interface/msg/state.hpp>

#define Pi  3.14159265358979323846


using namespace std::chrono_literals;
using namespace std::placeholders;


class PID : public rclcpp::Node
{
  public:
    PID() : Node("PID")
    {
      publisher = this->create_publisher<std_msgs::msg::Float64>("/Chessis_Input", 10);

      sub = this->create_subscription<std_msgs::msg::Float64>("/LOS_Angle", 10, std::bind(&PID::InputU, this, _1));
      sub2 = this->create_subscription<vrx_other_interface::msg::State>
                  ("/state", 10, std::bind(&PID::Control, this, _1));

      P = 100; I = 50; D = 15;
      LOS_ang = 0;
      Integtation = 0;
      t = 0;
      e_last = 0;
    }

  private:
    void Control(const vrx_other_interface::msg::State& msg)
    {
        double feedback = msg.pos.psi;
        float time = msg.time;
        float dt = time - t;
        t = time;

        double e = LOS_ang - feedback;
        Integtation += e*dt;
        double Deviation = (e-e_last)/dt;
        e = e_last;
        double u = P*e + I*Integtation + D*Deviation;

        auto pub_msg = std_msgs::msg::Float64();
        pub_msg.data = -u;
        publisher -> publish(pub_msg);
    }

    void InputU(const std_msgs::msg::Float64& msg)
    {
        LOS_ang = msg.data;

    }


    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr publisher;

    rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr sub;
    rclcpp::Subscription<vrx_other_interface::msg::State>::SharedPtr sub2;

    double P;
    double I;
    double D;
    double LOS_ang;
    double Integtation;
    float t;
    double e_last;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<PID>());
  rclcpp::shutdown();
  return 0;
}
