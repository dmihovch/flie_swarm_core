#include <ros/ros.h>
#include <sensor_msgs/Joy.h>
#include <math.h>
#include <algorithm>
#include <string>

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
    ros::Rate loop_rate(50);

    while(ros::ok() && ros::Time::now().toSec() == 0) {
        ros::spinOnce();
        ros::WallDuration(0.1).sleep();
    }

    ros::Time start_time = ros::Time::now();
    double phase_offset = (2.0 * M_PI * (drone_id - 1)) / swarm_size;
       
   
    while(ros::ok()) {
        sensor_msgs::Joy control_msg;
        control_msg.axes.resize(4);
        double t = (ros::Time::now() - start_time).toSec();
        
        if (t < 5.0) {
            control_msg.axes[0] = 0.0;
            control_msg.axes[1] = 0.0;
            control_msg.axes[2] = 2.0;
        } else if (t < 8.0) {
            control_msg.axes[0] = 0.0;
            control_msg.axes[1] = 0.0;
            control_msg.axes[2] = 0.0;
        } else {

            
            double swarm_time = t - 8.0;
            double blend = std::min(1.0,swarm_time/3.0);

            if(swarm_formation == "flying_v")
            {
                double forward_speed = 1.0;
                control_msg.axes[0] = forward_speed;
                control_msg.axes[1] = 0.0;
                control_msg.axes[2] = 0.0;
            }
            else if(swarm_formation == "helix")
            {
                double omega = 1.0;
                double radius = 2.0;
                control_msg.axes[0] = blend * -radius * omega * sin(omega * swarm_time + phase_offset);
                control_msg.axes[1] = blend * radius * omega * cos(omega * swarm_time + phase_offset);
                control_msg.axes[2] = blend * 0.2;
            }
            else if(swarm_formation == "carousel")
            {
                double omega = 0.5;
                double radius = 3.0;
                double z_amplitude = 0.5;
                control_msg.axes[0] = blend * -radius * omega * sin(omega * swarm_time + phase_offset);
                control_msg.axes[1] = blend * radius * omega * cos(omega * swarm_time + phase_offset);
                control_msg.axes[2] = blend * z_amplitude * cos(2.0 * omega * swarm_time + phase_offset);
            }
        }

        control_msg.axes[3] = 0.0;

        dji_control_pub.publish(control_msg);
        ros::spinOnce();
        loop_rate.sleep();
    }

    return 0;
}

