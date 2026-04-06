#include <ros/ros.h>
#include <sensor_msgs/Joy.h>
#include <math.h>

int main(int argc, char** argv) {
    ros::init(argc, argv, "ros_swarm_adapter_node");
    ros::NodeHandle nh;
    ros::NodeHandle nh_private("~");

    double x_dir = 1.0;
    double y_mult = 1.0;
    
    nh_private.param("x_dir",x_dir,1.0);
    nh_private.param("y_mult",y_mult, 1.0);

    ros::Publisher dji_control_pub = nh.advertise<sensor_msgs::Joy>("dji_sdk/flight_control_setpoint_ENUvelocity_yawrate",10);
    ros::Rate loop_rate(50);

    while(ros::ok() && ros::Time::now().toSec() == 0)
    {
        ros::spinOnce();
        ros::WallDuration(0.1).sleep();
    }

    ros::Time start_time = ros::Time::now();
    while(ros::ok())
    {
        sensor_msgs::Joy control_msg;
        control_msg.axes.resize(4);
        double t = (ros::Time::now() - start_time).toSec();
		
		double amplitude = 1;
		double frequency = 1;
		
		control_msg.axes[0] = x_dir * amplitude * cos(t * frequency);
		control_msg.axes[1] = y_mult * amplitude * sin(t * frequency);
		

        if(t < 5.0)
        {
            control_msg.axes[2] = 0.2;
        } else {
           control_msg.axes[2] = 0.0;
         }

        control_msg.axes[3] = 0.0;
        dji_control_pub.publish(control_msg);
        ros::spinOnce();
        loop_rate.sleep();

        
    }

    return 0;
}
