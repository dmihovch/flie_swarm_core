#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>

geometry_msgs::Twist current_cmd;

void odometryCallback(const nav_msgs::Odometry::ConstPtr& msg) {
    double z_pos = msg->pose.pose.position.z;
    
    if (z_pos < 2.0) {
        current_cmd.linear.z = 0.5;
        current_cmd.linear.x = 0.0;
    } else {
        current_cmd.linear.z = 0.0;
        current_cmd.linear.x = 0.5;
    }
}

void controlLoopCallback(const ros::TimerEvent&, ros::Publisher& pub) {
    pub.publish(current_cmd);
}

int main(int argc, char** argv) {
    ros::init(argc, argv, "ros_swarm_adapter_node");
    ros::NodeHandle nh;

    ros::Subscriber odom_sub = nh.subscribe("/swarm_member_1/ground_truth/odometry", 10, odometryCallback);
    ros::Publisher cmd_pub = nh.advertise<geometry_msgs::Twist>("/swarm_member_1/cmd_vel", 10);

    ros::Timer timer = nh.createTimer(ros::Duration(0.02), boost::bind(controlLoopCallback, _1, boost::ref(cmd_pub)));

    ros::spin();

    return 0;
}
