cmake_minimum_required(VERSION 3.8)
project(vrx_controller)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(vrx_other_interface REQUIRED)

include_directories(/include/vrx_controller)

add_executable(Go_Ctrl src/go_ahead.cpp)
ament_target_dependencies(Go_Ctrl rclcpp std_msgs)

add_executable(ADRC src/ADRC.cpp)
ament_target_dependencies(ADRC rclcpp std_msgs vrx_other_interface)

add_executable(Gain src/Pure_Gain.cpp)
ament_target_dependencies(Gain rclcpp std_msgs vrx_other_interface)

add_executable(ch_PID src/chessis_PID.cpp)
ament_target_dependencies(ch_PID rclcpp sensor_msgs std_msgs)

add_executable(LOS src/LOS.cpp)
ament_target_dependencies(LOS rclcpp Eigen3 std_msgs vrx_other_interface sensor_msgs)

add_executable(PID src/PID.cpp)
ament_target_dependencies(PID rclcpp std_msgs vrx_other_interface)

install(TARGETS
  Go_Ctrl
  ADRC
  ch_PID
  LOS
  Gain
  PID
  DESTINATION lib/${PROJECT_NAME})
  
  
if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
