#include <ros/ros.h>
#include <sensor_msgs/Joy.h>
#include <geometry_msgs/Twist.h>

class SimDjiBridge {
public:
    SimDjiBridge() {
        ros::NodeHandle nh;
        twist_pub_ = nh.advertise<geometry_msgs::Twist>("cmd_vel", 10);
        joy_sub_ = nh.subscribe("dji_sdk/flight_control_setpoint_ENUvelocity_yawrate", 10, &SimDjiBridge::joyCallback, this);
    }

private:
    void joyCallback(const sensor_msgs::Joy::ConstPtr& joy_msg) {
        if (joy_msg->axes.size() < 4) {
            return;
        }

        geometry_msgs::Twist twist_msg;
        twist_msg.linear.x = joy_msg->axes[0];
        twist_msg.linear.y = joy_msg->axes[1];
        twist_msg.linear.z = joy_msg->axes[2];
        
        twist_msg.angular.x = 0.0;
        twist_msg.angular.y = 0.0;
        twist_msg.angular.z = joy_msg->axes[3];

        twist_pub_.publish(twist_msg);
    }

    ros::Publisher twist_pub_;
    ros::Subscriber joy_sub_;
};

int main(int argc, char** argv) {
    ros::init(argc, argv, "sim_dji_bridge_node");
    SimDjiBridge bridge;
    ros::spin();
    return 0;
}
