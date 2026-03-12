//this is AI slop, just for testing and getting my grips
#include <ros/ros.h>
#include <dji_osdk_ros/FlightTaskControl.h>
#include <dji_osdk_ros/ObtainControlAuthority.h>

int main(int argc, char** argv) {
    ros::init(argc, argv, "dji_flight_test");
    ros::NodeHandle nh;

    // The default namespace for the DJI OSDK ROS node is usually /dji_osdk_ros
    std::string ns = "/dji_osdk_ros";

    ros::ServiceClient auth_client = nh.serviceClient<dji_osdk_ros::ObtainControlAuthority>(ns + "/obtain_release_control_authority");
    ros::ServiceClient task_client = nh.serviceClient<dji_osdk_ros::FlightTaskControl>(ns + "/flight_task_control");

    ROS_INFO("Waiting for DJI OSDK services...");
    auth_client.waitForExistence();
    task_client.waitForExistence();

    // 1. Obtain Control Authority
    ROS_INFO("Obtaining control authority...");
    dji_osdk_ros::ObtainControlAuthority auth_srv;
    auth_srv.request.enable_obtain = true;
    
    if (auth_client.call(auth_srv) && auth_srv.response.result) {
        ROS_INFO("Control authority obtained.");
    } else {
        ROS_ERROR("Failed to obtain control authority.");
        return 1;
    }

    // 2. Takeoff
    ROS_INFO("Executing takeoff...");
    dji_osdk_ros::FlightTaskControl task_srv;
    task_srv.request.task = dji_osdk_ros::FlightTaskControl::Request::TASK_TAKEOFF;
    
    if (task_client.call(task_srv) && task_srv.response.result) {
        ROS_INFO("Takeoff successful.");
    } else {
        ROS_ERROR("Takeoff failed.");
        return 1;
    }

    // 3. Hover (The drone automatically hovers after a takeoff task completes)
    ROS_INFO("Hovering for 5 seconds...");
    ros::Duration(5.0).sleep();

    // 4. Land
    ROS_INFO("Executing landing...");
    task_srv.request.task = dji_osdk_ros::FlightTaskControl::Request::TASK_LAND;
    
    if (task_client.call(task_srv) && task_srv.response.result) {
        ROS_INFO("Landing successful.");
    } else {
        ROS_ERROR("Landing failed.");
        return 1;
    }

    return 0;
}
