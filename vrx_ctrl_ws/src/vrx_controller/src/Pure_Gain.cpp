#include <chrono>
#include <functional>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64.hpp"
#include <vrx_other_interface/msg/state.hpp>

using namespace std::chrono_literals;
using namespace std::placeholders;

/* This example creates a subclass of Node and uses std::bind() to register a
* member function as a callback from the timer. */

class Pure_Gain : public rclcpp::Node
{
  public:
    Pure_Gain() : Node("Pure_Gain")
    {
      publisher = this->create_publisher<std_msgs::msg::Float64>("/Chessis_Input", 10);

      sub = this->create_subscription<std_msgs::msg::Float64>("/LOS_Angle", 10, std::bind(&Pure_Gain::InputU, this, _1));
      state_sub = this->create_subscription<vrx_other_interface::msg::State>
                  ("/state", 10, std::bind(&Pure_Gain::Control, this, _1));
      
      P = 100;
      LOS_ang = 0;
    }

  private:
    void InputU(const std_msgs::msg::Float64& msg)
    {
        LOS_ang = msg.data;
    }

    void Control(const vrx_other_interface::msg::State& msg)
    {
        double feedback_psi = msg.pos.psi;
        double u = -P * (LOS_ang - feedback_psi);           // 保证 负反馈

        auto pub_msg = std_msgs::msg::Float64();
        pub_msg.data = u;
        publisher-> publish(pub_msg);
    }

    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr publisher;
    rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr sub;
    rclcpp::Subscription<vrx_other_interface::msg::State>::SharedPtr state_sub;

    double P;
    double LOS_ang;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<Pure_Gain>());
  rclcpp::shutdown();
  return 0;
}
