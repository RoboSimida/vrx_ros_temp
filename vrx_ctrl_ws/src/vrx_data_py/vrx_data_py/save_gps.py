import rclpy
from rclpy.node import Node
import numpy as np

from sensor_msgs.msg import NavSatFix


class Gps_Saver(Node):

    def __init__(self):
        super().__init__('Gps_Saver')
        self.subscription = self.create_subscription(
            NavSatFix, '/wamv/sensors/gps/gps/fix', self.listener_callback, 10)
            
        self.subscription  # prevent unused variable warning
        self.count = 0
        self.data = np.zeros((1000, 5))
        
        self.lat0 = -33.7227687
        self.lon0 = 150.67399115
        self.RE = 6371393

    def listener_callback(self, msg):
    	latitude = msg.latitude
    	longitude = msg.longitude
    	sec = msg.header.stamp.sec
    	nanosec = msg.header.stamp.nanosec
    	time = sec + nanosec/1000000000.0
        
        # forget to convert deg to rad in measure psi0 , remember to modify
    	x = (longitude - self.lon0) / 180 * np.pi * self.RE
    	y = (latitude - self.lat0)  / 180 * np.pi * self.RE
    	self.count += 1
    	if self.count < len(self.data):
            self.data[self.count, 0] = time
            self.data[self.count, 1] = longitude
            self.data[self.count, 2] = latitude
            self.data[self.count, 3] = x
            self.data[self.count, 4] = y
    	if self.count == len(self.data):
            np.savetxt('/home/cizar/Desktop/NeedRename.txt', self.data)
    		
    	self.get_logger().info('Count "%d"' % self.count)


def main(args=None):
    rclpy.init(args=args)

    minimal_subscriber = Gps_Saver()

    rclpy.spin(minimal_subscriber)

    # Destroy the node explicitly
    # (optional - otherwise it will be done automatically
    # when the garbage collector destroys the node object)
    minimal_subscriber.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()
