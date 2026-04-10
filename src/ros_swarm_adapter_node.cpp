#include <ros/ros.h>
#include <sensor_msgs/Joy.h>
#include <nav_msgs/Odometry.h>
#include <math.h>
#include <algorithm>
#include <string>

#define IDLE_TIME 5.0
#define UPWARD_THRUST_TIME IDLE_TIME + 3.0
#define SLOW_HOVER_TIME UPWARD_THRUST_TIME + 3.0
#define T_RESET SLOW_HOVER_TIME

double current_x = 0.0;
double current_y = 0.0;
double current_z = 0.0;
bool odom_received = false;

void odomCallback(const nav_msgs::Odometry::ConstPtr& msg) {
    current_x = msg->pose.pose.position.x;
    current_y = msg->pose.pose.position.y;
    current_z = msg->pose.pose.position.z;
    odom_received = true;
}

int main(int argc, char** argv) {
    ros::init(argc, argv, "ros_swarm_adapter_node");
    ros::NodeHandle nh;
    ros::NodeHandle nh_private("~");

    int drone_id = 1;
    int swarm_size = 1;
    std::string swarm_formation = "flying_v";

    nh_private.param("drone_id", drone_id, 1);
    nh_private.param("swarm_size", swarm_size, 1);
    nh_private.param<std::string>("swarm_formation", swarm_formation, "flying_v");

    ros::Publisher dji_control_pub = nh.advertise<sensor_msgs::Joy>("dji_sdk/flight_control_setpoint_ENUvelocity_yawrate", 10);
    ros::Subscriber odom_sub = nh.subscribe("ground_truth/state", 10, odomCallback);
    ros::Rate loop_rate(50);

    while(ros::ok() && ros::Time::now().toSec() == 0) {
        ros::spinOnce();
        ros::WallDuration(0.1).sleep();
    }

    ros::Time start_time = ros::Time::now();

    while(ros::ok()) {
        sensor_msgs::Joy control_msg;
        control_msg.axes.resize(4);
        double t = (ros::Time::now() - start_time).toSec();

        if (t < IDLE_TIME) {
            control_msg.axes[0] = 0.0;
            control_msg.axes[1] = 0.0;
            control_msg.axes[2] = 0.0;
        } else if (t < UPWARD_THRUST_TIME) {
            control_msg.axes[0] = 0.0;
            control_msg.axes[1] = 0.0;
            if (swarm_formation == "helix") {
                control_msg.axes[2] = 1.0 + ((drone_id - 1) * 0.2);
            } else {
                control_msg.axes[2] = 2.0;
            }
        } 
        else if (t < SLOW_HOVER_TIME){
            
            control_msg.axes[0] = 0.0;
            control_msg.axes[1] = 0.0;
            control_msg.axes[2] = 0.2;
        }
        else {
            double swarm_time = t - 8.0;

            if(swarm_formation == "flying_v") {
                double forward_speed = 2.0;
                control_msg.axes[0] = forward_speed;
                control_msg.axes[1] = 0.0;
                control_msg.axes[2] = 0.0;
            }
            else if(swarm_formation == "helix") {
                double phase_offset = (2.0 * M_PI * (drone_id - 1)) / swarm_size;
                double target_omega = 0.5;
                double radius = 3.0;

                double current_omega;
                double current_angle;

                if (swarm_time < 3.0) {
                    current_omega = target_omega * (swarm_time / 3.0);
                    current_angle = (target_omega / 6.0) * swarm_time * swarm_time + phase_offset;
                } else {
                    current_omega = target_omega;
                    current_angle = 1.5 * target_omega + target_omega * (swarm_time - 3.0) + phase_offset;
                }

                double target_x = radius * cos(current_angle);
                double target_y = radius * sin(current_angle);
                double Kp = 0.5;

                double err_x = odom_received ? (target_x - current_x) : 0.0;
                double err_y = odom_received ? (target_y - current_y) : 0.0;

                double ff_vx = -radius * current_omega * sin(current_angle);
                double ff_vy = radius * current_omega * cos(current_angle);

                control_msg.axes[0] = ff_vx + (Kp * err_x);
                control_msg.axes[1] = ff_vy + (Kp * err_y);
                control_msg.axes[2] = 0.2;
            }
            else if(swarm_formation == "carousel") {
                double phase_offset = (2.0 * M_PI * (drone_id - 1)) / swarm_size;
                double target_omega = 0.5;
                double radius = 3.0;
                double z_amplitude = 0.5;

                double current_omega;
                double current_angle;

                if (swarm_time < 3.0) {
                    current_omega = target_omega * (swarm_time / 3.0);
                    current_angle = (target_omega / 6.0) * swarm_time * swarm_time + phase_offset;
                } else {
                    current_omega = target_omega;
                    current_angle = 1.5 * target_omega + target_omega * (swarm_time - 3.0) + phase_offset;
                }

                double target_x = radius * cos(current_angle);
                double target_y = radius * sin(current_angle);
                double Kp = 0.5;

                double err_x = odom_received ? (target_x - current_x) : 0.0;
                double err_y = odom_received ? (target_y - current_y) : 0.0;

                double ff_vx = -radius * current_omega * sin(current_angle);
                double ff_vy = radius * current_omega * cos(current_angle);

                control_msg.axes[0] = ff_vx + (Kp * err_x);
                control_msg.axes[1] = ff_vy + (Kp * err_y);
                control_msg.axes[2] = z_amplitude * cos(3.0 * current_angle);
            }
            else {
                control_msg.axes[0] = 0.0;
                control_msg.axes[1] = 0.0;
                control_msg.axes[2] = 0.0;
            }
        }

        control_msg.axes[3] = 0.0;

        dji_control_pub.publish(control_msg);
        ros::spinOnce();
        loop_rate.sleep();
    }

    return 0;
}



