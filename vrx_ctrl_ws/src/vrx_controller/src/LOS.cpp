#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <cmath>

#include <eigen3/Eigen/Dense>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64.hpp"
#include <vrx_other_interface/msg/state.hpp>
#include <vrx_other_interface/msg/losmsg.hpp>
#include "sensor_msgs/msg/joint_state.hpp"


#define Pi  3.14159265

using namespace std::chrono_literals;
using namespace std::placeholders;
using namespace Eigen;


class LOS_Navigation : public rclcpp::Node
{
  public:
    LOS_Navigation()
    : Node("LOS_Navigation")
    {
      thR_publisher = this->create_publisher<std_msgs::msg::Float64>("/wamv/thrusters/right/thrust", 10);
      thL_publisher = this->create_publisher<std_msgs::msg::Float64>("/wamv/thrusters/left/thrust", 10);
      ch_publisher = this->create_publisher<std_msgs::msg::Float64>("/LOS_Angle", 10);
      e_publisher = this->create_publisher<vrx_other_interface::msg::Losmsg>("/error", 10);

      state_sub = this->create_subscription<vrx_other_interface::msg::State>(
      "/state", 10, std::bind(&LOS_Navigation::LOS, this, _1));
	/*
      joint_sub = this->create_subscription<sensor_msgs::msg::JointState>(
      "/wamv/joint_states", 10, std::bind(&LOS_Navigation::Ang_Update, this, _1));
	*/
      IsEnd = false;

      direct << 0, 0;
      poslos << 0, 0;

      R = 2; dx = 15;
      idx = 0;

      last_time = 0;
      sum_error = 0;
      // Ang_Pos = 0;
    }

    template <class T>
    int len(T& arr)
    {
        return (sizeof(arr) / sizeof(arr[0]));
    }


  private:
    void LOS(const vrx_other_interface::msg::State& msg)
    {
        double sec = msg.time;
        double dt = sec - last_time;
        last_time = sec;

        double x = msg.pos.x;
        double y = msg.pos.y;
        double psi = msg.pos.psi;
        double u = msg.vel.u;
        double v = msg.vel.v;

        psi += std::atan2(v, u);

        psi = std::fmod(psi, 2*Pi);
        if(psi > Pi)
            psi -= 2*Pi;
        if(psi < -Pi)
            psi += 2*Pi;

        Vector2d pos(x, y);
        Vector2d site(sitex[idx], sitey[idx]);


        // Also can try use pure C(for interest )
        if((poslos-pos).norm() < R)
        {
            if((pos-site).norm() < R)
            {
                if(idx < len(sitex)-1)
                {
                    poslos << sitex[idx], sitey[idx];
                    idx ++;
                    direct << (sitex[idx]-sitex[idx-1]), (sitey[idx]-sitey[idx-1]);
                    direct = direct/direct.norm();
                }
                if(idx == len(sitex)-1)
                {
                    poslos << sitex[idx-1], sitey[idx-1];
                    direct << (x-sitex[idx]), (y-sitey[idx]);
                    IsEnd = true;
                }
            }
            else
            {
                poslos = poslos + direct*dx;
                if(poslos(0)>sitex[idx])
                {
                	poslos << sitex[idx], sitey[idx];
                	if(idx < len(sitex)-1)
                        idx ++;
                    direct << (sitex[idx]-sitex[idx-1]), (sitey[idx]-sitey[idx-1]);
                    direct = direct/direct.norm();
                }
            }
        }

        site << sitex[idx], sitey[idx];
        if((idx == len(sitex)-1) && ((pos-site).norm() <  R))
        {
            IsEnd = true;
        }

        double LOS_Ang = 0;
        if((poslos(0)-x) == 0)
            LOS_Ang = Pi/2;
        else
            LOS_Ang = std::atan2(poslos(1)-y, poslos(0)-x);

        double e = LOS_Ang - psi;
        if(e > Pi)  e -= 2*Pi;
        else if(e < -Pi)  e += 2*Pi;

        LOS_Ang = psi + e;


        double n;
        double error;
        // value 0 when n, error declare and ignore IsEnd condition maybe clean
        // but it will warning "variable ‘xx’ set but not used"
        if(IsEnd)
        {
            n = 0;
            error = 0;
        }
        else
        {
            double k = direct(1)/direct(0); // flaw: ignore den=0
            n = 100;
            error = std::fabs(y-poslos(1) - k*(x-poslos(0)))/std::sqrt(1+std::pow(k, 2));
        }

        sum_error += error * dt;

        auto n_msg = std_msgs::msg::Float64();
        auto l_msg = std_msgs::msg::Float64();
        auto e_msg = vrx_other_interface::msg::Losmsg();
        n_msg.data = n;
        l_msg.data = LOS_Ang;
        e_msg.error = sum_error;
        e_msg.end = IsEnd;

        thR_publisher -> publish(n_msg);
        thL_publisher -> publish(n_msg);
        ch_publisher ->  publish(l_msg);
        e_publisher -> publish(e_msg);

        RCLCPP_INFO_STREAM(this->get_logger(), "\n" << poslos(0) << "  " << (pos-site).norm()<< " " << idx << len(sitex));

    }
	/*
    void Ang_Update(const sensor_msgs::msg::JointState& msg)
    {
        Ang_Pos = msg.position[4];
    } */

    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr thR_publisher;
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr thL_publisher;
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr ch_publisher;
    rclcpp::Publisher<vrx_other_interface::msg::Losmsg>::SharedPtr e_publisher;

    rclcpp::Subscription<vrx_other_interface::msg::State>::SharedPtr state_sub;
    // rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr joint_sub;


    double sitex[3] = {0, 10, 22};
    double sitey[3] = {0, 10, 14};
    bool IsEnd;

    Vector2d direct;
    Vector2d poslos;

    double R;
    double dx;
    int idx;

    double last_time;
    double sum_error;

    // double Ang_Pos = 0;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<LOS_Navigation>());
  rclcpp::shutdown();
  return 0;
}
