cmake_minimum_required(VERSION 3.8)
project(vrx_experiment)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(std_msgs REQUIRED)
find_package(rosgraph_msgs REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)

add_executable(measureT src/measure_joint_T.cpp)
ament_target_dependencies(measureT rclcpp std_msgs)

add_executable(slow_speed_up src/slow_speed_up.cpp)
ament_target_dependencies(slow_speed_up rclcpp std_msgs rosgraph_msgs)

add_executable(step_speed_up src/step_speed_up.cpp)
ament_target_dependencies(step_speed_up rclcpp std_msgs rosgraph_msgs)

add_executable(round src/round.cpp)
ament_target_dependencies(round rclcpp std_msgs)

add_executable(SysIden src/SysIden.cpp)
ament_target_dependencies(SysIden rclcpp std_msgs rosgraph_msgs)

add_executable(Fang src/Fang.cpp)
ament_target_dependencies(Fang rclcpp std_msgs sensor_msgs)


install(TARGETS
  measureT
  slow_speed_up
  step_speed_up
  SysIden
  round
  Fang
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
