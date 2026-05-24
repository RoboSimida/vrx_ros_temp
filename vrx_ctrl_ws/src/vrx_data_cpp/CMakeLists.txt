cmake_minimum_required(VERSION 3.8)
project(vrx_data_cpp)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(rclcpp REQUIRED)
find_package(vrx_other_interface REQUIRED)

add_executable(gps_imu_fuse src/gps_imu_fuse.cpp)
ament_target_dependencies(gps_imu_fuse rclcpp Eigen3 sensor_msgs vrx_other_interface)
add_executable(check_sign src/check_sign.cpp)
ament_target_dependencies(check_sign rclcpp sensor_msgs vrx_other_interface)

add_executable(fuse_substisute src/fuse_substisute.cpp)
ament_target_dependencies(fuse_substisute rclcpp sensor_msgs vrx_other_interface)


install(TARGETS
  gps_imu_fuse
  check_sign
  fuse_substisute
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
