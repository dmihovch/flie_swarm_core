cmake_minimum_required(VERSION 3.0.2)
project(flie_swarm_core)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  dji_osdk_ros
)

catkin_package()

include_directories(
  ${catkin_INCLUDE_DIRS}
)

add_executable(ros_swarm_adapter_node src/ros_swarm_adapter_node.cpp)
target_link_libraries(ros_swarm_adapter_node ${catkin_LIBRARIES})
add_dependencies(ros_swarm_adapter_node ${catkin_EXPORTED_TARGETS})
