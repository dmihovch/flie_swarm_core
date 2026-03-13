#include <ros/ros.h>
#include <geometry_msgs/TwistStamped.h>
#include <nav_msgs/Odometry.h>

geometry_msgs::TwistStamped current_cmd;

void odometryCallback(const nav_msgs::Odometry::ConstPtr& msg) {
    double z_pos = msg->pose.pose.position.z;
    
    if (z_pos < 2.0) {
        current_cmd.twist.linear.z = 0.5;
        current_cmd.twist.linear.x = 0.0;
		current_cmd.twist.linear.y = 0.0;
    } else {
        current_cmd.twist.linear.z = 0.0;
        current_cmd.twist.linear.x = 0.2;
		current_cmd.twist.linear.y = 0.0;
    }

    ROS_INFO_THROTTLE(1.0, "Altitude: %.2f m | Cmd Z: %.1f | Cmd X: %.1f", z_pos, current_cmd.twist.linear.z, current_cmd.twist.linear.x);
}

void controlLoopCallback(const ros::TimerEvent&, ros::Publisher& pub) {
    current_cmd.header.stamp = ros::Time::now();
    current_cmd.header.frame_id = "base_stabilized";
    pub.publish(current_cmd);
}

int main(int argc, char** argv) {
    ros::init(argc, argv, "ros_swarm_adapter_node");
    ros::NodeHandle nh;

    ros::Subscriber odom_sub = nh.subscribe("/swarm_member_1/ground_truth/odometry", 10, odometryCallback);
    ros::Publisher cmd_pub = nh.advertise<geometry_msgs::TwistStamped>("/swarm_member_1/command/twist", 10);

    ros::Timer timer = nh.createTimer(ros::Duration(0.02), boost::bind(controlLoopCallback, _1, boost::ref(cmd_pub)));

    ros::spin();

    return 0;
}
