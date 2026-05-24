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
        
        self.th5 = 1.54861
        self.th6 = -0.0274
        self.th7 = 0.136
        self.acc_z = np.zeros((1, 5))
        self.declare_parameter('Length_of_Data', 300)
        
        self.len_data = self.get_parameter('Length_of_Data').value
        self.data1 = np.zeros((self.len_data, 5))
        self.data2 = np.zeros((self.len_data, 5))
        
    def save_callback(self, msg):
    	u = msg.vel.u
    	v = msg.vel.v
    	r = msg.vel.psi/0.1
    	av = msg.acc.v
    	ar = msg.acc.psi
    	# ar = np.mean(np.append(self.acc_z[1:], ar))
    	
    	V = u*np.cos(self.ang)+v*np.sin(self.ang)
    	U = np.maximum(np.sqrt(u**2+v**2), 0.01)
    	sin = np.sin(self.ang)
    	n = self.n/100
    	
    	th_5 = 1/self.th5
    	th6_5 = self.th6/self.th5
    	th7_5 = self.th7/self.th5
    	
    	self.count += 1
    	if self.count < self.len_data:    
    	    temp = -n**2*sin
    	    
    	    self.data1[self.count, 0] = av
    	    self.data1[self.count, 1] = v
    	    self.data1[self.count, 2] = u*r*th_5 + temp
    	    
    	    self.data2[self.count, 0] = ar
    	    self.data2[self.count, 1] = r
    	    self.data2[self.count, 2] = temp*1.027
    	    
    	    self.get_logger().info('Count "%d"' % self.count)
    	if self.count == self.len_data:
            np.savetxt('/home/cizar/Desktop/NeedRename_IRLSv.txt', self.data1)
            np.savetxt('/home/cizar/Desktop/NeedRename_IRLSr.txt', self.data2)
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
