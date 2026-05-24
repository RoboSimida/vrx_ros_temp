import rclpy
from rclpy.node import Node
import numpy as np

from sensor_msgs.msg import JointState


class JointState_Saver(Node):

    def __init__(self):
        super().__init__('JointState_Saver')
        self.subscription = self.create_subscription(
            JointState, '/wamv/joint_states', self.listener_callback, 10)
            
        self.subscription  # prevent unused variable warning
        self.count = 0
        self.declare_parameter('Length_of_Data', 1000)
        
        self.len_data = self.get_parameter('Length_of_Data').value
        self.data = np.zeros((self.len_data, 9))
        
    def listener_callback(self, msg):
    	sec = msg.header.stamp.sec
    	nanosec = msg.header.stamp.nanosec
    	time = sec + nanosec/1000000000.0
    	position = msg.position
    	velocity = msg.velocity
        
    	self.count += 1
    	if self.count < self.len_data:
            self.data[self.count, 0] = time
            self.data[self.count, 1] = position[4]
            self.data[self.count, 2] = position[5]
            self.data[self.count, 3] = position[7]
            self.data[self.count, 4] = position[8]
            self.data[self.count, 5] = velocity[4]
            self.data[self.count, 6] = velocity[5]
            self.data[self.count, 7] = velocity[7]
            self.data[self.count, 8] = velocity[8]
            # self.get_logger().info('Count "%d"' % self.count)
            self.get_logger().info('Count "%d"' % self.count)
    	if self.count == self.len_data:
            np.savetxt('/home/cizar/Desktop/NeedRename.txt', self.data)
            self.get_logger().info('Save Joint Txt')

def main(args=None):
    rclpy.init(args=args)

    minimal_subscriber = JointState_Saver()

    rclpy.spin(minimal_subscriber)
    minimal_subscriber.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()
