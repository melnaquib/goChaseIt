#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // TODO: Request a service and pass the velocities to it to drive the robot
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;
    
    client.call(srv);    
}

void drive_to(int height, int width, int idx) {
	
	const float VEL_LIN_X_FACTOR = 5.0, VEL_ANG_Z_FACTOR = 5.0;
	
	int col = idx % width;
	
	if(col < width / 3.0)
			drive_robot(0, VEL_ANG_Z_FACTOR);
	else if(col < width * 2.0 / 3.0)
		drive_robot(0, -1.0 * VEL_ANG_Z_FACTOR);
	else
		drive_robot(0, -1.0 * VEL_ANG_Z_FACTOR);
}

void drive_stop() {
	drive_robot(0, 0);
}
		
// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;

    // TODO: Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera
    
    int i = 0;
    for (i = 0; i < img.height * img.step ; ++i) {
			if (white_pixel == img.data[i]) {
				drive_to(i, img.height, img.step);
				break;
			}
		}
		if( (img.height * img.step) <= i) //loop totally consumed; no white_point found 
			drive_stop();
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
