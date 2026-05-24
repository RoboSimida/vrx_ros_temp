import rclpy
from rclpy.node import Node
import numpy as np

from sensor_msgs.msg import JointState
from vrx_other_interface.msg import State


class IPLS_Straight_Saver(Node):

    def __init__(self):
        super().__init__('IPLS_Straight_Saver')
        self.joint_sub = self.create_subscription(
            JointState, '/wamv/joint_states', self.joint_callback, 10)
        self.state_sub = self.create_subscription(
            State, '/state', self.save_callback, 10)
            
        self.joint_sub   # prevent unused variable warning
        self.state_sub
        
        self.n = 0
        self.ang = 0
        self.count = 0
        self.declare_parameter('Length_of_Data', 300)
        
        self.len_data = self.get_parameter('Length_of_Data').value
        self.data = np.zeros((self.len_data, 5))
        
    def save_callback(self, msg):
    	u = msg.vel.u
    	v = msg.vel.v
    	r = msg.vel.psi/0.1
    	au = msg.acc.u
    	V = u*np.cos(self.ang)+v*np.sin(self.ang)
    	U = np.maximum(np.sqrt(u**2+v**2), 0.01)
    	cos = np.cos(self.ang)
    	n = self.n/100
    	self.count += 1
    	if self.count < self.len_data:
    	    self.data[self.count, 0] = au
    	    self.data[self.count, 1] = u #np.abs(u)*u # np.sign(u)*U*U
    	    self.data[self.count, 2] = v*r
    	    self.data[self.count, 3] = n**2*cos
    	    self.get_logger().info('Count "%d"' % self.count)
    	if self.count == self.len_data:
            np.savetxt('/home/cizar/Desktop/NeedRename_IRLS.txt', self.data)
            self.get_logger().info('Save IRLS Txt')
    	        
    def joint_callback(self, msg):
    	self.ang = msg.position[4]
    	self.n = msg.velocity[5]
    	
            
        

def main(args=None):
    rclpy.init(args=args)

    minimal_subscriber = IPLS_Straight_Saver()

    rclpy.spin(minimal_subscriber)
    minimal_subscriber.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()
