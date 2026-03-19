#include <ros/ros.h>
#include <sensor_msgs/Joy.h>

int main(int argc, char** argv) {
    ros::init(argc, argv, "ros_swarm_adapter_node");
    ros::NodeHandle nh;

    ros::Publisher dji_control_pub = nh.advertise<sensor_msgs::Joy>("/dji_sdk/flight_control_setpoint_ENUvelocity_yawrate", 10);

    ros::Rate loop_rate(50);

    while (ros::ok()) {
        sensor_msgs::Joy control_msg;
        control_msg.axes.resize(4);

        control_msg.axes[0] = 0.5;
        control_msg.axes[1] = 0.0;
        control_msg.axes[2] = 0.0;
        control_msg.axes[3] = 0.0;

        dji_control_pub.publish(control_msg);

        ros::spinOnce();
        loop_rate.sleep();
    }

    return 0;
}
