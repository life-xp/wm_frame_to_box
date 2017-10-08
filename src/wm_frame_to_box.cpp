//
// Created by philippe on 07/10/17.
//

#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <tf/transform_broadcaster.h>
#include <darknet_ros_msgs/BoundingBoxes.h>


darknet_ros_msgs::BoundingBoxes BoundingBoxes2D;






// from: https://answers.ros.org/question/90696/get-depth-from-kinect-sensor-in-gazebo-simulator/
typedef union U_FloatParse {
    float float_data;
    unsigned char byte_data[4];
} U_FloatConvert;
int ReadDepthData(unsigned int height_pos, unsigned int width_pos, sensor_msgs::ImageConstPtr depth_image)
{
    // If position is invalid
    if ((height_pos >= depth_image->height) || (width_pos >= depth_image->width))
        return -1;
    int index = (height_pos*depth_image->step) + (width_pos*(depth_image->step/depth_image->width));
    // If data is 4 byte floats (rectified depth image)
    if ((depth_image->step/depth_image->width) == 4) {
        U_FloatConvert depth_data;
        int i, endian_check = 1;
        // If big endian
        if ((depth_image->is_bigendian && (*(char*)&endian_check != 1)) ||  // Both big endian
            ((!depth_image->is_bigendian) && (*(char*)&endian_check == 1))) { // Both lil endian
            for (i = 0; i < 4; i++)
                depth_data.byte_data[i] = depth_image->data[index + i];
            // Make sure data is valid (check if NaN)
            if (depth_data.float_data == depth_data.float_data)
                return int(depth_data.float_data*1000);
            return -1;  // If depth data invalid
        }
        // else, one little endian, one big endian
        for (i = 0; i < 4; i++)
            depth_data.byte_data[i] = depth_image->data[3 + index - i];
        // Make sure data is valid (check if NaN)
        if (depth_data.float_data == depth_data.float_data)
            return int(depth_data.float_data*1000);
        return -1;  // If depth data invalid
    }
    // Otherwise, data is 2 byte integers (raw depth image)
    int temp_val;
    // If big endian
    if (depth_image->is_bigendian)
        temp_val = (depth_image->data[index] << 8) + depth_image->data[index + 1];
        // If little endian
    else
        temp_val = depth_image->data[index] + (depth_image->data[index + 1] << 8);
    // Make sure data is valid (check if NaN)
    if (temp_val == temp_val)
        return temp_val;
    return -1;  // If depth data invalid
}






void DNBB( darknet_ros_msgs::BoundingBoxes msg ){

    BoundingBoxes2D = msg;
}



void ImageCB(const sensor_msgs::ImageConstPtr& msg){

    ulong L = BoundingBoxes2D.boundingBoxes.size();
    for (ulong i=0; i < L; i++ ){
        int xmin = BoundingBoxes2D.boundingBoxes[i].xmin;
        int xmax = BoundingBoxes2D.boundingBoxes[i].xmax;
        int ymin = BoundingBoxes2D.boundingBoxes[i].ymin;
        int ymax = BoundingBoxes2D.boundingBoxes[i].ymax;
        int x = (xmax+xmin)/2;
        int y = (ymax+ymin)/2;
        if (x<0) x=0;
        if (x>msg->width) x=msg->width;
        if (y<0) y=0;
        if (y>msg->height) y=msg->height;



        ROS_INFO("x = %d", x );
        ROS_INFO("y = %d", y );

        double dist = ReadDepthData( (uint)x, (uint)y, msg )/1000.0;
        ROS_INFO("dist = %f", dist );
        static tf::TransformBroadcaster br;
        tf::Transform transform;
        transform.setOrigin( tf::Vector3(dist, -((double)x-(double)msg->width/2)*0.0018*dist, -((double)y-(double)msg->height/2)*0.0018*dist) );
        tf::Quaternion q;
        q.setRPY(0, 0, 0);
        transform.setRotation(q);
        br.sendTransform(tf::StampedTransform(transform, ros::Time::now(), "head_xtion_depth_frame", BoundingBoxes2D.boundingBoxes[i].Class));
    }

}


int main(int argc, char **argv) {

    ros::init(argc, argv, "frame_to_box");

    ros::NodeHandle nh;
    image_transport::ImageTransport it(nh);
    image_transport::Subscriber sub = it.subscribe("/head_xtion/depth/image_raw", 1, ImageCB);

    ros::Subscriber bbsub = nh.subscribe("/darknet_ros/bounding_boxes", 1, DNBB);

    ros::spin();
}
