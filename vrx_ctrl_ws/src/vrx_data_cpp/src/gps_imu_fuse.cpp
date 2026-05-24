#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <cmath>
#include <algorithm>

#include <eigen3/Eigen/Dense>

#include "rclcpp/rclcpp.hpp"
#include <sensor_msgs/msg/nav_sat_fix.hpp>
#include <sensor_msgs/msg/imu.hpp>
#include <vrx_other_interface/msg/state.hpp>

#define Pi  3.14159265358979323846



using namespace std::chrono_literals;
using namespace std::placeholders;

typedef Eigen::Matrix<double, 2, 1> Vector2d;
typedef Eigen::Matrix<double, 2, 2> Matrix22d;
typedef Eigen::Matrix<double, 8, 8> Matrix88d;
typedef Eigen::Matrix<double, 2, 8> Matrix28d;
typedef Eigen::Matrix<double, 8, 2> Matrix82d;

/*
  Attributiton  num     index
  Pos x, y      2       (0, 0, 2, 1);
  Vel x, y      2       (2, 0, 2, 1);
  Psi           1       (4, 1);
  Acc_b u,v     2       (5, 0, 2, 1);
  Ags_b         1       (7, 0);
  Vector use segment
  */
typedef Eigen::Vector<double, 8> VectorSt;
typedef struct
{
  double time;
  VectorSt Value;
} state;



class gps_imu_fuse : public rclcpp::Node
{
  public:
    gps_imu_fuse()
    : Node("gps_imu_fuse"), lat0(-33.72276868), lon0(150.673991172), RE(6371393)
    {
      gps_sub = this->create_subscription<sensor_msgs::msg::NavSatFix>(
       "/wamv/sensors/gps/gps/fix", 10, std::bind(&gps_imu_fuse::GPS_Callback, this, _1));

      imu_sub = this->create_subscription<sensor_msgs::msg::Imu>(
      "/wamv/sensors/imu/imu/data", 10, std::bind(&gps_imu_fuse::IMU_Callback, this, _1));

      state_publisher = this->create_publisher<vrx_other_interface::msg::State>("state", 10);
      // timer_ = this->create_wall_timer(100ms, std::bind(&gps_imu_fuse::Callback, this));

      last_w = 0;

      VectorSt Init_State;
      Init_State << 0, 0, 0, 0, 0.91424, 0, 0, 0;// 0, 0, 0, 0, 0.91424, -1.554e-1, 5.342e-2, 0;
      Nominal_State = {0, Init_State};
      Error_State = {0, VectorSt::Zero()};

      K = Matrix82d::Zero();
      P = Matrix88d::Identity() * 0.5;
      V << 3e-7,        -1.586e-7,
           -1.586e-7,   9.374e-8;
      V *= 1e6;
      H << 1, 0, 0, 0, 0, 0, 0, 0,
           0, 1, 0, 0, 0, 0, 0, 0;
      Cov_a << 4.668e-4, 7.262e-6,
               7.262e-6, 4.171e-4;
      avcheck = 0;
    }

  private:
    void IMU_Callback(const sensor_msgs::msg::Imu& imu)
    {
      // imu data time
      auto t = imu.header.stamp;
      double sec = t.sec + t.nanosec/1000000000.0;
      double dt = sec- Nominal_State.time;

      // imu data & convert Quaternion to Euler
      auto a = imu.linear_acceleration;
      auto w = imu.angular_velocity;


      // some basic value
      Matrix88d F = Matrix88d::Identity();
      Matrix88d Q = Matrix88d::Identity();
      Matrix22d R;                          // Rotation Matrix
      Matrix22d R_dot;
      Matrix22d R_inv;
      Matrix22d L;                          // L matrix
      Vector2d am;                          // measured accerlation
      double wm = w.z;                      // measured angular speed
      double Psi = Nominal_State.Value(4);

      R << std::cos(Psi), -std::sin(Psi),
           std::sin(Psi), std::cos(Psi);
      R_dot << -std::sin(Psi), -std::cos(Psi),
                std::cos(Psi), -std::sin(Psi);
      R_inv <<   std::cos(Psi), std::sin(Psi),
                -std::sin(Psi), std::cos(Psi);
      L << 0, -1,
           1,  0;
      am << a.x, a.y;				// imu frame is different with assumption


      Vector2d da = am - Nominal_State.Value.segment(5, 2);
      double dw = wm - Nominal_State.Value(7);
      Vector2d Ra = R * da;
      Vector2d RinvV = R_inv * Nominal_State.Value.segment(2, 2);

      F.block(0, 2, 2, 2) = Matrix22d::Identity() * dt;                     // p->v
      F.block(2, 2, 2, 2) += R_dot*R_inv*dw * dt;                           // v->v(plus)
      F.block(2, 4, 2, 1) = (R*L*da + R_dot*L*RinvV*dw) * dt;               // v->psi
      F.block(2, 5, 2, 2) = -R * dt;                                        // v->ab
      F.block(2, 7, 2, 1) = -R_dot*RinvV * dt;                              // v->wb
      F(4, 7) = -dt;                                                        // psi->wb


      // Nominal Update
      Nominal_State.time = sec;
      Nominal_State.Value.segment(0, 2) += Nominal_State.Value.segment(2, 2)*dt; // pos
      Nominal_State.Value.segment(2, 2) += (Ra+R_dot*RinvV*dw) * dt;             // vel
      Nominal_State.Value(4) += dw*dt;                                           // ags_b

      // Error Update
      Vector2d R2V = R_dot * RinvV;
      Q = Generate_Q(dt, &R, R2V(0), R2V(1));
      P = F * P * F.transpose() + Q;

      // publish message
      VectorSt state_now = Nominal_State.Value + Error_State.Value;
      double vx = state_now(2);
      double vy = state_now(3);
      double aub = state_now(5);
      double avb = state_now(6);
      double wzb = state_now(7);

      auto message = vrx_other_interface::msg::State();
      message.time = sec;
      message.pos.x = state_now(0);
      message.pos.y = state_now(1);
      message.pos.psi = state_now(4);
      message.vel.u = vx * std::cos(Psi) + vy * std::sin(Psi);
      message.vel.v = -vx * std::sin(Psi) + vy * std::cos(Psi);
      message.vel.psi = wm - wzb;
      message.acc.u = am(0) - aub;
      message.acc.v = am(1) - avb;
      message.acc.psi = (wm - wzb - last_w) / dt;

      state_publisher->publish(message);

      // Update last_w
      last_w = wm - wzb;

      avcheck = am(1);

    }


    void GPS_Callback(const sensor_msgs::msg::NavSatFix& gps)
    {
      // gps data time
      auto t = gps.header.stamp;
      double sec = t.sec + t.nanosec/1000000000.0;

      // gps data
      double latitude = gps.latitude;
      double longitude = gps.longitude;
      double x = (longitude - lon0) / 180 * RE * Pi;
      double y = (latitude - lat0) / 180 * RE * Pi;

      // Kalman Update
      Vector2d Inspect = Vector2d(x, y);
      K = P*H.transpose()*(H*P*H.transpose()+V).inverse();

      VectorSt state_t = Nominal_State.Value;
      VectorSt delta_x = K*(Inspect - H*state_t);

      // delta_x(2) = 0;
      // delta_x(3) = 0;
      // delta_x(4) = 0;
      // delta_x(5) = 0;
      // delta_x(6) = 0;
      // delta_x(7) = 0;
      Nominal_State.Value += delta_x;

      auto I_KH = Matrix88d::Identity() - K*H;
      P = I_KH * P;

      //RCLCPP_INFO_STREAM(this->get_logger(), "\n" << P);
      /*
      RCLCPP_INFO_STREAM(this->get_logger(),
                "\nw: " << last_w + state_t(7) << " wb:" << state_t(7) <<
                "\navb: " << Nominal_State.Value(6) << "av: " << avcheck); */
      // RCLCPP_INFO_STREAM(this->get_logger(), "\nHPH :\n" << H*P*H.transpose() <<
      //            "\nV:\n" << V <<
      //            "\n(HPH+V):\n" << (H*P*H.transpose()+V) <<
      //            "\n(HPH+V)-1:\n" << (H*P*H.transpose()+V).inverse() <<
      //            "\nK:\n" << K);


      /*
      it may make dt=0 in imu_update,
      nominal and error not same time lead to little error
      // Nominal_State.time = sec;
      */

      // ESKF Correct and Reset
      // delta_x(2) = std::min(0.1, std::max(-0.1, delta_x(2)));
      // delta_x(3) = std::min(0.1, std::max(-0.1, delta_x(3)));
      // double wb_ = Nominal_State.Value(7);
      // Nominal_State.Value(5) = std::min(0.1, std::max(-0.1, Nominal_State.Value(5)));
      // Nominal_State.Value(6) = std::min(0.1, std::max(-0.1, Nominal_State.Value(6)));
      // Nominal_State.Value(7) = std::min(0.1, std::max(-0.1, wb_));


      Error_State.time = sec;
      Error_State.Value = VectorSt::Zero();
    }

    Matrix88d Generate_Q(double dt, Matrix22d *R, double a1, double a2)
    {
      double Dwb = 8.697e-5;
      Matrix88d Q = Matrix88d:: Zero();
      Q(0, 0) = 1e-5;
      Q(1, 1) = 1e-5;
      Q.block(2, 2, 2, 2) = *R * (*R).transpose()* Cov_a * dt;
      Q(2, 2) += a1 * a1 * Dwb * dt;
      Q(3, 3) += a2 * a2 * Dwb * dt;
      Q(4, 4) = Dwb * dt * dt;
      Q.block(5, 5, 2, 2) = Cov_a;
      Q(7, 7) = Dwb;           // D(wb)

      return Q;
    }

    rclcpp::Subscription<sensor_msgs::msg::NavSatFix>::SharedPtr gps_sub;
    rclcpp::Subscription<sensor_msgs::msg::Imu>::SharedPtr imu_sub;

    rclcpp::Publisher<vrx_other_interface::msg::State>::SharedPtr state_publisher;

    const double lat0;
    const double lon0;
    const long RE;

    double last_w;
    double avcheck;

    state Nominal_State;
    state Error_State;
    Matrix82d K;
    Matrix88d P;
    Matrix22d V;
    Matrix28d H;
    Matrix22d Cov_a;

};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<gps_imu_fuse>());
  rclcpp::shutdown();
  return 0;
}



/*
    Message Filter Example (Make Two Topic Receive Nearly the Same Time)
    // #include "message_filters/subscriber.h"
    // #include <message_filters/time_synchronizer.h>
    // gps_sub = message_filters::Subscriber<sensor_msgs::msg::NavSatFix>(this, "/wamv/sensors/gps/gps/fix");
    // imu_sub = message_filters::Subscriber<sensor_msgs::msg::Imu>(this, "/wamv/sensors/imu/imu/data");
    // sync = message_filters::TimeSynchronizer<sensor_msgs::msg::NavSatFix, sensor_msgs::msg::Imu>(gps_sub, imu_sub, 16);
    // sync.registerCallback(std::bind(&gps_imu_fuse::Callback, _1, _2));
*/
