#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>

ros::Publisher cmd_pub;
geometry_msgs::Twist current_cmd;
bool odom_init = false;
double start_alt = 0.0;

void odometryCallback(const nav_msgs::Odometry::ConstPtr& msg) {
    if (!odom_init) {
        start_alt = msg->pose.pose.position.z;
        odom_init = true;
    }

    double z_pos = msg->pose.pose.position.z;

    if (z_pos < start_alt + 2.0) {
        current_cmd.linear.z = 0.5;
        current_cmd.linear.x = 0.0;
    } else {
        current_cmd.linear.z = 0.0;
        current_cmd.linear.x = 0.5;
    }

    cmd_pub.publish(current_cmd);
    ROS_INFO_THROTTLE(1.0, "Alt: %.2f m | Cmd Z: %.1f | Cmd X: %.1f", z_pos, current_cmd.linear.z, current_cmd.linear.x);
}

int main(int argc, char** argv) {
    ros::init(argc, argv, "ros_swarm_adapter_node");
    ros::NodeHandle nh;

    cmd_pub = nh.advertise<geometry_msgs::Twist>("cmd_vel", 10);
    ros::Subscriber odom_sub = nh.subscribe("ground_truth/state", 10, odometryCallback);

    ros::spin();
    return 0;
}
