import rclpy
from rclpy.node import Node
import numpy as np

from std_msgs.msg import Float64

class Input_Saver(Node):

    def __init__(self):
        super().__init__('Input_Saver')
        self.thrust_sub = self.create_subscription(
            Float64, "/wamv/thrusters/right/thrust", self.thrust_callback, 10)
        self.pos_sub = self.create_subscription(
            Float64, "/wamv/thrusters/right/pos", self.pos_callback, 10)
            
        self.thrust_sub   # prevent unused variable warning
        self.pos_sub
        self.count_t = 0	# thruster
        self.count_p = 0	# pos
        self.declare_parameter('Length_of_Data_Input', 300)
        
        self.len_data = self.get_parameter('Length_of_Data_Input').value
        self.data_t = np.zeros((self.len_data, 1))
        self.data_p = np.zeros((self.len_data, 1))
        
    def thrust_callback(self, msg):
    	temp = msg.data
        
    	self.count_t += 1
    	if self.count_t < self.len_data:
            self.data_t[self.count_t, 0] = temp
            self.get_logger().info('Count "%d"' % self.count_t)
    	if self.count_t == self.len_data:
            np.savetxt('/home/cizar/Desktop/NeedRename_Input_Thruster.txt', self.data_t)
            self.get_logger().info('Save Input_Thruster Txt')
            
    def pos_callback(self, msg):
    	temp = msg.data
        
    	self.count_p += 1
    	if self.count_p < self.len_data:
            self.data_p[self.count_p, 0] = temp
    	if self.count_p == self.len_data:
            np.savetxt('/home/cizar/Desktop/NeedRename_Input_Pos.txt', self.data_p)
            self.get_logger().info('Save Input_Pos Txt')
        

def main(args=None):
    rclpy.init(args=args)

    minimal_subscriber = Input_Saver()

    rclpy.spin(minimal_subscriber)
    minimal_subscriber.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()
