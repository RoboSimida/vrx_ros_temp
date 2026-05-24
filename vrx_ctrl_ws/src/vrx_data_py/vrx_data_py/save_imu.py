import rclpy
from rclpy.node import Node
import numpy as np

from sensor_msgs.msg import Imu


class Imu_Saver(Node):

    def __init__(self):
        super().__init__('Imu_Saver')
        self.subscription = self.create_subscription(
            Imu, "/wamv/sensors/imu/imu/data", self.listener_callback, 10)
            
        self.subscription  # prevent unused variable warning
        self.count = 0
        self.data = np.zeros((1000, 4))

    def listener_callback(self, imu):
    	sec = imu.header.stamp.sec
    	nanosec = imu.header.stamp.nanosec
    	time = sec + nanosec/1000000000.0
    	au = imu.linear_acceleration.x
    	av = imu.linear_acceleration.y
    	wz = imu.angular_velocity.z
        
    	
    	if self.count < len(self.data):
            self.data[self.count, 0] = time
            self.data[self.count, 1] = au
            self.data[self.count, 2] = av
            self.data[self.count, 3] = wz
            
            self.count += 1
            self.get_logger().info('Count "%d"' % self.count)
            
    	if self.count == len(self.data):
            np.savetxt('/home/cizar/Desktop/NeedRename.txt', self.data)
            self.get_logger().info('Save TXT')  
    	


def main(args=None):
    rclpy.init(args=args)

    minimal_subscriber = Imu_Saver()

    rclpy.spin(minimal_subscriber)

    # Destroy the node explicitly
    # (optional - otherwise it will be done automatically
    # when the garbage collector destroys the node object)
    minimal_subscriber.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()
